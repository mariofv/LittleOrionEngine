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

#include "FrameBuffer.h"
#include "MultiSampledFrameBuffer.h"

Viewport::Viewport(int options) : viewport_options(options)
{
	blit_fbo = new FrameBuffer();
	regular_fbo = new FrameBuffer();
	multisampled_fbo = new MultiSampledFrameBuffer();

	main_fbo = regular_fbo;
}

Viewport::~Viewport()
{
	delete blit_fbo;
	delete regular_fbo;
	delete multisampled_fbo;
}

void Viewport::Render(ComponentCamera* camera)
{
	this->camera = camera;
	camera->SetAspectRatio(width / height);
	BindCameraMatrices();

	culled_mesh_renderers = App->space_partitioning->GetCullingMeshes(camera, App->renderer->mesh_renderers);

	LightCameraPass();
	MeshRenderPass();
	EffectsRenderPass();
	UIRenderPass();
	DebugPass();
	DebugDrawPass();
	EditorDrawPass();
	PostProcessPass();

	SelectLastDisplayedTexture();
}

void Viewport::BindCameraMatrices() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

	static size_t projection_matrix_offset = App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET + sizeof(float4x4);
	glBufferSubData(GL_UNIFORM_BUFFER, projection_matrix_offset, sizeof(float4x4), camera->GetProjectionMatrix().Transposed().ptr());

	static size_t view_matrix_offset = App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET + 2 * sizeof(float4x4);
	glBufferSubData(GL_UNIFORM_BUFFER, view_matrix_offset, sizeof(float4x4), camera->GetViewMatrix().Transposed().ptr());

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Frustum Viewport::InitLightFrustum(const float3& position, const float3& up, const float3& front, float vertical_fov, float horizontal_fov, float near_distance, float far_distance) const
{
	Frustum light_frustum = Frustum();
	light_frustum.type = FrustumType::PerspectiveFrustum;
	light_frustum.pos = position;
	light_frustum.front = front;
	light_frustum.up = up;
	light_frustum.nearPlaneDistance = near_distance;
	light_frustum.farPlaneDistance = far_distance;
	light_frustum.verticalFov = vertical_fov;
	light_frustum.horizontalFov = horizontal_fov;

	return light_frustum;
}

void Viewport::LightCameraPass() const
{
	if (!shadows_pass || App->cameras->main_camera == nullptr)
	{
		return;
	}

	float3 game_camera_position = App->cameras->main_camera->owner->transform.GetGlobalTranslation();
	float3 game_camera_up = App->cameras->main_camera->owner->transform.GetGlobalUpVector();
	float3 game_camera_front = App->cameras->main_camera->owner->transform.GetGlobalFrontVector();

	float vertical_fov = App->cameras->main_camera->camera_frustum.verticalFov;
	float horizontal_fov = App->cameras->main_camera->camera_frustum.horizontalFov;

	float game_camera_near_distance = App->cameras->main_camera->GetNearDistance();
	float game_camera_far_distance = App->cameras->main_camera->GetFarDistance();
	float game_camera_frustum_depth = game_camera_far_distance - game_camera_near_distance;
	
	float game_camera_first_third = game_camera_near_distance + game_camera_frustum_depth / 3.f;
	float game_camera_second_third = game_camera_far_distance - game_camera_frustum_depth / 3.f;
	
	Frustum near_frustum = InitLightFrustum(game_camera_position, game_camera_up, game_camera_front, vertical_fov, horizontal_fov, game_camera_near_distance, game_camera_first_third);
	Frustum mid_frustum = InitLightFrustum(game_camera_position, game_camera_up, game_camera_front, vertical_fov, horizontal_fov, game_camera_near_distance, game_camera_second_third);
	Frustum far_frustum = InitLightFrustum(game_camera_position, game_camera_up, game_camera_front, vertical_fov, horizontal_fov, game_camera_near_distance, game_camera_far_distance);

	App->debug_draw->RenderPerspectiveFrustum(near_frustum.ViewProjMatrix().Inverted(), float3::unitX);
	App->debug_draw->RenderPerspectiveFrustum(mid_frustum.ViewProjMatrix().Inverted(), float3::unitY);
	App->debug_draw->RenderPerspectiveFrustum(far_frustum.ViewProjMatrix().Inverted(), float3::unitZ);
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

	blit_fbo->ClearAttachements();
	blit_fbo->GenerateAttachements(width, height);
	
	regular_fbo->ClearAttachements();
	regular_fbo->GenerateAttachements(width, height);

	multisampled_fbo->ClearAttachements();
	multisampled_fbo->GenerateAttachements(width, height);
}

void Viewport::SelectLastDisplayedTexture()
{
	last_displayed_texture = blit_fbo->GetColorAttachement();
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
