#include "Viewport.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleEffects.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Helper/Utils.h"

#include "FrameBuffer/DepthFrameBuffer.h"
#include "FrameBuffer/FrameBuffer.h"
#include "FrameBuffer/MultiSampledFrameBuffer.h"
#include "LightFrustum.h"

Viewport::Viewport(int options) : viewport_options(options)
{
	framebuffers.emplace_back(blit_fbo = new FrameBuffer());
	framebuffers.emplace_back(regular_fbo = new FrameBuffer());
	framebuffers.emplace_back(multisampled_fbo = new MultiSampledFrameBuffer());

	depth_near_fbo = new FrameBuffer();
	depth_mid_fbo = new FrameBuffer();
	depth_far_fbo = new FrameBuffer();

	near_frustum = new LightFrustum(LightFrustum::FrustumSubDivision::NEAR_FRUSTUM);
	mid_frustum = new LightFrustum(LightFrustum::FrustumSubDivision::MID_FRUSTUM);
	far_frustum = new LightFrustum(LightFrustum::FrustumSubDivision::FAR_FRUSTUM);

	main_fbo = regular_fbo;
}

Viewport::~Viewport()
{
	for (auto& framebuffer : framebuffers)
	{
		delete framebuffer;
	}

	delete depth_near_fbo;
	delete depth_mid_fbo;
	delete depth_far_fbo;

	delete near_frustum;
	delete mid_frustum;
	delete far_frustum;
}

void Viewport::Render(ComponentCamera* camera)
{
	this->camera = camera;
	camera->SetAspectRatio(width / height);

	culled_mesh_renderers = App->space_partitioning->GetCullingMeshes(camera, App->renderer->mesh_renderers);

	LightCameraPass();

	BindCameraFrustumMatrices(camera->camera_frustum);
	MeshRenderPass();
	EffectsRenderPass();
	UIRenderPass();
	DebugPass();
	DebugDrawPass();
	EditorDrawPass();
	PostProcessPass();

	SelectLastDisplayedTexture();
}

void Viewport::BindCameraFrustumMatrices(const Frustum& camera_frustum) const
{
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

	static size_t projection_matrix_offset = App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET + sizeof(float4x4);
	float4x4 projection_matrix = camera_frustum.ProjectionMatrix();
	glBufferSubData(GL_UNIFORM_BUFFER, projection_matrix_offset, sizeof(float4x4), projection_matrix.Transposed().ptr());

	static size_t view_matrix_offset = App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET + 2 * sizeof(float4x4);
	float4x4 view_matrix = camera_frustum.ViewMatrix();
	glBufferSubData(GL_UNIFORM_BUFFER, view_matrix_offset, sizeof(float4x4), view_matrix.Transposed().ptr());

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Viewport::LightCameraPass() const
{
	if (!shadows_pass || App->cameras->main_camera == nullptr)
	{
		return;
	}

	near_frustum->Update();
	depth_near_fbo->ClearAttachements();
	depth_near_fbo->GenerateAttachements(near_frustum->light_orthogonal_frustum.orthographicWidth, near_frustum->light_orthogonal_frustum.orthographicHeight);
	near_frustum->RenderLightFrustum();

	mid_frustum->Update();
	depth_mid_fbo->ClearAttachements();
	depth_mid_fbo->GenerateAttachements(mid_frustum->light_orthogonal_frustum.orthographicWidth, mid_frustum->light_orthogonal_frustum.orthographicHeight);
	mid_frustum->RenderLightFrustum();

	far_frustum->Update();
	depth_far_fbo->ClearAttachements();
	depth_far_fbo->GenerateAttachements(far_frustum->light_orthogonal_frustum.orthographicWidth, far_frustum->light_orthogonal_frustum.orthographicHeight);
	far_frustum->RenderLightFrustum();

	depth_near_fbo->Bind();
	glClearColor(1.f, 1.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

	static size_t projection_matrix_offset = App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET + sizeof(float4x4);
	float4x4 projection_matrix = float4x4::identity;
	glBufferSubData(GL_UNIFORM_BUFFER, projection_matrix_offset, sizeof(float4x4), projection_matrix.Transposed().ptr());

	static size_t view_matrix_offset = App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET + 2 * sizeof(float4x4);
	float4x4 view_matrix = float4x4::identity;
	glBufferSubData(GL_UNIFORM_BUFFER, view_matrix_offset, sizeof(float4x4), view_matrix.Transposed().ptr());

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);	for (ComponentMeshRenderer* culled_mesh_renderer : culled_mesh_renderers)
	{
		if (culled_mesh_renderer->shadow_caster)
		{
			culled_mesh_renderer->Render();
			glUseProgram(0);
		}
	}
	FrameBuffer::UnBind();


	depth_mid_fbo->Bind();
	glClearColor(1.f, 1.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	BindCameraFrustumMatrices(mid_frustum->light_orthogonal_frustum);
	for (ComponentMeshRenderer* culled_mesh_renderer : culled_mesh_renderers)
	{
		if (culled_mesh_renderer->shadow_caster)
		{
			culled_mesh_renderer->Render();
			glUseProgram(0);
		}
	}
	FrameBuffer::UnBind();


	depth_far_fbo->Bind();
	glClearColor(1.f, 1.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	BindCameraFrustumMatrices(far_frustum->light_orthogonal_frustum);
	for (ComponentMeshRenderer* culled_mesh_renderer : culled_mesh_renderers)
	{
		if (culled_mesh_renderer->shadow_caster)
		{
			culled_mesh_renderer->Render();
			glUseProgram(0);
		}
	}
	FrameBuffer::UnBind();
	glClearColor(0.f, 0.f, 0.f, 1.f);
}

void Viewport::MeshRenderPass() const
{
	main_fbo->Bind();
	camera->Clear();

	float3 camera_position = camera->owner->transform.GetGlobalTranslation();

	std::vector<Utils::MeshRendererDistancePair> opaque_mesh_renderers;
	std::vector<Utils::MeshRendererDistancePair> transparent_mesh_renderers;
	Utils::SplitCulledMeshRenderers(culled_mesh_renderers, camera_position, opaque_mesh_renderers, transparent_mesh_renderers);

	for (auto& opaque_mesh_renderer : opaque_mesh_renderers)
	{
		if (opaque_mesh_renderer.mesh_renderer->mesh_to_render != nullptr && opaque_mesh_renderer.mesh_renderer->IsEnabled())
		{
			opaque_mesh_renderer.mesh_renderer->Render();
			/*
			num_rendered_tris += mesh.second->mesh_to_render->GetNumTriangles();
			num_rendered_verts += mesh.second->mesh_to_render->GetNumVerts();
			App->lights->UpdateLightAABB(*mesh.second->owner);
			*/
			glUseProgram(0);
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	for (auto& transparent_mesh_renderer : transparent_mesh_renderers)
	{
		if (transparent_mesh_renderer.mesh_renderer->mesh_to_render != nullptr && transparent_mesh_renderer.mesh_renderer->IsEnabled())
		{
			transparent_mesh_renderer.mesh_renderer->Render();
			/*
			num_rendered_tris += mesh.second->mesh_to_render->GetNumTriangles();
			num_rendered_verts += mesh.second->mesh_to_render->GetNumVerts();
			App->lights->UpdateLightAABB(*mesh.second->owner);
			*/
			glUseProgram(0);
		}
	}

	FrameBuffer::UnBind();
}

void Viewport::EffectsRenderPass() const
{
	if (!effects_pass)
	{
		return;
	}

	main_fbo->Bind();
	App->effects->Render();
	FrameBuffer::UnBind();
}

void Viewport::UIRenderPass() const
{
	main_fbo->Bind();
	App->ui->Render(width, height, IsOptionSet(ViewportOption::SCENE_MODE));
	FrameBuffer::UnBind();
}

void Viewport::PostProcessPass() const
{
	main_fbo->Bind(GL_READ_FRAMEBUFFER);

	if (IsOptionSet(ViewportOption::BLIT_FRAMEBUFFER))
	{
		blit_fbo->Bind(GL_DRAW_FRAMEBUFFER);
	}
	else
	{
		FrameBuffer::UnBind(GL_DRAW_FRAMEBUFFER);
	}

	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	FrameBuffer::UnBind();

	//App->renderer->RenderPostProcessingEffects(*camera);
}

void Viewport::DebugPass() const
{
	if (!debug_pass || !IsOptionSet(ViewportOption::SCENE_MODE))
	{
		return;
	}

	App->debug->Render();

}

void Viewport::DebugDrawPass() const
{
	if (!debug_draw_pass)
	{
		return;
	}

	main_fbo->Bind();
	App->debug_draw->Render(width, height, camera->GetProjectionMatrix() * camera->GetViewMatrix());
	FrameBuffer::UnBind();
}

void Viewport::EditorDrawPass() const
{
	if (!IsOptionSet(ViewportOption::SCENE_MODE))
	{
		return;
	}

	main_fbo->Bind();
	App->debug_draw->RenderGrid();
	if (App->debug->show_navmesh)
	{
		App->debug_draw->RenderNavMesh(*camera);
	}
	App->debug_draw->RenderBillboards();
	if (App->editor->selected_game_object != nullptr)
	{
		App->debug_draw->RenderOutline();
	}
	FrameBuffer::UnBind();
}

void Viewport::SetSize(float width, float height)
{
	if (this->width == width && this->height == height)
	{
		return;
	}

	this->width = width;
	this->height = height;
	glViewport(0, 0, width, height);

	for (auto& framebuffer : framebuffers)
	{
		framebuffer->ClearAttachements();
		framebuffer->GenerateAttachements(width, height);
	}
}

void Viewport::SelectLastDisplayedTexture()
{
	switch (viewport_output)
	{
	case ViewportOutput::COLOR:
		last_displayed_texture = blit_fbo->GetColorAttachement();
		break;

	case ViewportOutput::DEPTH_NEAR:
		last_displayed_texture = depth_near_fbo->GetColorAttachement();
		break;

	case ViewportOutput::DEPTH_MID:
		last_displayed_texture = depth_mid_fbo->GetColorAttachement();
		break;

	case ViewportOutput::DEPTH_FAR:
		last_displayed_texture = depth_far_fbo->GetColorAttachement();
		break;

	default:
		break;
	}

}

bool Viewport::IsOptionSet(ViewportOption option) const
{
	return viewport_options & (int)option;
}

void Viewport::SetAntialiasing(bool antialiasing)
{
	this->antialiasing = antialiasing;
	if (antialiasing)
	{
		main_fbo = multisampled_fbo;
	}
	else
	{
		main_fbo = regular_fbo;
	}
}

void Viewport::SetOutput(ViewportOutput output)
{
	viewport_output = output;
}
