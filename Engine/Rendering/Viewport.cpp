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
#include "LightFrustum.h"

Viewport::Viewport(int options) : viewport_options(options)
{
	scene_quad = new Quad(2.f);

	framebuffers.emplace_back(main_fbo = new FrameBuffer(2));
	framebuffers.emplace_back(ping_fbo = new FrameBuffer());
	framebuffers.emplace_back(pong_fbo = new FrameBuffer());
	framebuffers.emplace_back(postprocess_fbo = new FrameBuffer());
	framebuffers.emplace_back(blit_fbo = new FrameBuffer());

	depth_full_fbo = new DepthFrameBuffer();
	depth_near_fbo = new DepthFrameBuffer();
	depth_mid_fbo = new DepthFrameBuffer();
	depth_far_fbo = new DepthFrameBuffer();
}

Viewport::~Viewport()
{
	delete scene_quad;

	for (auto& framebuffer : framebuffers)
	{
		delete framebuffer;
	}

	delete depth_full_fbo;
	delete depth_near_fbo;
	delete depth_mid_fbo;
	delete depth_far_fbo;
}

void Viewport::Render(ComponentCamera* camera)
{
	this->camera = camera;
	camera->SetAspectRatio(width / height);
	culled_mesh_renderers = App->space_partitioning->GetCullingMeshes(camera, App->renderer->mesh_renderers);

	LightCameraPass();
	App->lights->BindLightFrustumsMatrices();

	BindCameraFrustumMatrices(camera->camera_frustum);
	glViewport(0, 0, width, height);

	MeshRenderPass();
	EffectsRenderPass();
	DebugPass();
	EditorDrawPass();
	DebugDrawPass();
	PostProcessPass();
	UIRenderPass();
	BlitPass();

	SelectDisplayedTexture();
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

void Viewport::BindDepthMaps(GLuint program) const
{
	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_2D, depth_full_fbo->GetColorAttachement());
	glUniform1i(glGetUniformLocation(program, "close_depth_map"), 12);

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, depth_mid_fbo->GetColorAttachement());
	glUniform1i(glGetUniformLocation(program, "mid_depth_map"), 13);

	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, depth_full_fbo->GetColorAttachement());
	glUniform1i(glGetUniformLocation(program, "far_depth_map"), 14);
}

void Viewport::LightCameraPass() const
{
	if (!shadows_pass || App->cameras->main_camera == nullptr)
	{
		return;
	}

	DepthMapPass(App->lights->full_frustum, depth_full_fbo);
	DepthMapPass(App->lights->near_frustum, depth_near_fbo);
	DepthMapPass(App->lights->mid_frustum, depth_mid_fbo);
	DepthMapPass(App->lights->far_frustum, depth_far_fbo);
}

void Viewport::MeshRenderPass() const
{
	main_fbo->Bind();
	main_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT0, camera->camera_clear_color);
	main_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT1);

	if (camera->HasSkybox())
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		SkyboxPass();
	}

	static GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	float3 camera_position = camera->owner->transform.GetGlobalTranslation();

	std::vector<Utils::MeshRendererDistancePair> opaque_mesh_renderers;
	std::vector<Utils::MeshRendererDistancePair> transparent_mesh_renderers;
	Utils::SplitCulledMeshRenderers(culled_mesh_renderers, camera_position, opaque_mesh_renderers, transparent_mesh_renderers);

	for (auto& opaque_mesh_renderer : opaque_mesh_renderers)
	{
		if (opaque_mesh_renderer.mesh_renderer->mesh_to_render != nullptr 
			&& opaque_mesh_renderer.mesh_renderer->material_to_render != nullptr
			&& opaque_mesh_renderer.mesh_renderer->IsEnabled()
		)
		{
			GLuint mesh_renderer_program = opaque_mesh_renderer.mesh_renderer->BindShaderProgram();
			opaque_mesh_renderer.mesh_renderer->BindMeshUniforms(mesh_renderer_program);
			opaque_mesh_renderer.mesh_renderer->BindMaterialUniforms(mesh_renderer_program);
			BindDepthMaps(mesh_renderer_program);
			App->lights->Render(opaque_mesh_renderer.mesh_renderer->owner->transform.GetGlobalTranslation(), mesh_renderer_program);
			opaque_mesh_renderer.mesh_renderer->RenderModel();
			/*
			num_rendered_tris += mesh.second->mesh_to_render->GetNumTriangles();
			num_rendered_verts += mesh.second->mesh_to_render->GetNumVerts();
			*/
			glUseProgram(0);
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	for (auto& transparent_mesh_renderer : transparent_mesh_renderers)
	{
		if (transparent_mesh_renderer.mesh_renderer->mesh_to_render != nullptr
			&& transparent_mesh_renderer.mesh_renderer->material_to_render != nullptr
			&& transparent_mesh_renderer.mesh_renderer->IsEnabled()
		) {
			GLuint mesh_renderer_program = transparent_mesh_renderer.mesh_renderer->BindShaderProgram();
			transparent_mesh_renderer.mesh_renderer->BindMeshUniforms(mesh_renderer_program);
			transparent_mesh_renderer.mesh_renderer->BindMaterialUniforms(mesh_renderer_program);
			BindDepthMaps(mesh_renderer_program);
			App->lights->Render(transparent_mesh_renderer.mesh_renderer->owner->transform.GetGlobalTranslation(), mesh_renderer_program);
			transparent_mesh_renderer.mesh_renderer->RenderModel();
			/*
			num_rendered_tris += mesh.second->mesh_to_render->GetNumTriangles();
			num_rendered_verts += mesh.second->mesh_to_render->GetNumVerts();
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
	postprocess_fbo->Bind();
	App->ui->Render(width, height, IsOptionSet(ViewportOption::SCENE_MODE));
	FrameBuffer::UnBind();
}

void Viewport::PostProcessPass()
{
	BloomPass();
	HDRPass();
}

void Viewport::BlitPass() const
{
	postprocess_fbo->Bind(GL_READ_FRAMEBUFFER);

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
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

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
	unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

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


void Viewport::SkyboxPass() const
{
	App->cameras->world_skybox->Render(*camera);
}

void Viewport::DepthMapPass(LightFrustum* light_frustum, FrameBuffer* depth_fbo) const
{
	depth_fbo->ClearAttachements();
	depth_fbo->GenerateAttachements(width, height);

	light_frustum->RenderMeshRenderersAABB();
	light_frustum->RenderLightFrustum();

	depth_fbo->Bind();
	BindCameraFrustumMatrices(light_frustum->light_orthogonal_frustum);

	depth_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT0);;
	glViewport(0, 0, width, height);

	std::vector<ComponentMeshRenderer*> culled_shadow_casters = App->space_partitioning->GetCullingMeshes(
		App->cameras->main_camera,
		App->renderer->mesh_renderers,
		ComponentMeshRenderer::MeshProperties::SHADOW_CASTER
	);

	for (ComponentMeshRenderer* culled_shadow_caster : culled_shadow_casters)
	{
		if (
			culled_shadow_caster->mesh_to_render != nullptr
			&& culled_shadow_caster->material_to_render != nullptr
			&& culled_shadow_caster->IsEnabled()
			) {
			GLuint mesh_renderer_program = culled_shadow_caster->BindDepthShaderProgram();
			culled_shadow_caster->BindMeshUniforms(mesh_renderer_program);
			culled_shadow_caster->RenderModel();
			glUseProgram(0);
		}
	}
	FrameBuffer::UnBind();
}

void Viewport::BloomPass()
{
	if (!App->renderer->bloom)
	{
		return;
	}

	FrameBuffer* current_fbo = ping_fbo;
	FrameBuffer* other_fbo = ping_fbo;

	bool horizontal = true;
	bool first_iteration = true;

	GLuint shader_program = App->program->UseProgram("Blur", 0);
	for (unsigned int i = 0; i < App->renderer->amount_of_blur; i++)
	{
		current_fbo->Bind();
		glUniform1f(glGetUniformLocation(shader_program, "horizontal"), horizontal);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? main_fbo->GetColorAttachement(1) : other_fbo->GetColorAttachement());
		glUniform1i(glGetUniformLocation(shader_program, "image"), 0);

		scene_quad->Render();

		horizontal = !horizontal;
		std::swap(current_fbo, other_fbo);
		if (first_iteration)
		{
			first_iteration = false;
		}
	}

	glUseProgram(0);
	FrameBuffer::UnBind();

	ping_pong_fbo = other_fbo;
}

void Viewport::HDRPass() const
{
	postprocess_fbo->Bind();
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	int shader_variation = 0;
	if (antialiasing)
	{
		shader_variation |= (int)ModuleProgram::ShaderVariation::ENABLE_MSAA;
	}
	if (hdr)
	{
		shader_variation |= (int)ModuleProgram::ShaderVariation::ENABLE_HDR;
		switch (App->renderer->hdr_type)
		{
		case ModuleRender::HDRType::REINHARD:
			shader_variation |= (int)ModuleProgram::ShaderVariation::ENABLE_HDR;
			break;

		case ModuleRender::HDRType::FILMIC:
			shader_variation |= (int)ModuleProgram::ShaderVariation::ENABLE_FILMIC;
			break;

		case ModuleRender::HDRType::EXPOSURE:
			shader_variation |= (int)ModuleProgram::ShaderVariation::ENABLE_EXPOSURE;
			break;
		}
	}
	if (App->renderer->bloom)
	{
		shader_variation |= (int)ModuleProgram::ShaderVariation::ENABLE_BLOOM;
	}
	GLuint program = App->program->UseProgram("PostProcessing", shader_variation);

	glActiveTexture(GL_TEXTURE0);
	if (antialiasing)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, main_fbo->GetColorAttachement());
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, main_fbo->GetColorAttachement());
	}
	glUniform1i(glGetUniformLocation(program, "screen_texture"), 0);
	glUniform1f(glGetUniformLocation(program, "exposure"), App->renderer->exposure);

	if (App->renderer->bloom)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ping_pong_fbo->GetColorAttachement());
		glUniform1i(glGetUniformLocation(program, "brightness_texture"), 1);
	}

	scene_quad->Render();

	glUseProgram(0);
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

	for (auto& framebuffer : framebuffers)
	{
		framebuffer->ClearAttachements();
		framebuffer->GenerateAttachements(width, height);
	}
}

void Viewport::SelectDisplayedTexture()
{
	switch (viewport_output)
	{
	case ViewportOutput::COLOR:
		displayed_texture = blit_fbo->GetColorAttachement();
		break;

	case ViewportOutput::BRIGHTNESS:
		displayed_texture = main_fbo->GetColorAttachement(1);
		break;

	case ViewportOutput::DEPTH_NEAR:
		displayed_texture = depth_near_fbo->GetColorAttachement();
		break;

	case ViewportOutput::DEPTH_MID:
		displayed_texture = depth_mid_fbo->GetColorAttachement();
		break;

	case ViewportOutput::DEPTH_FAR:
		displayed_texture = depth_far_fbo->GetColorAttachement();
		break;

	case ViewportOutput::DEPTH_FULL:
		displayed_texture = depth_full_fbo->GetColorAttachement();
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
	main_fbo->SetMultiSampled(antialiasing);
	main_fbo->ClearAttachements();
	main_fbo->GenerateAttachements(width, height);
}

void Viewport::SetHDR(bool hdr)
{
	this->hdr = hdr;

	main_fbo->SetFloatingPoint(hdr);
	main_fbo->ClearAttachements();
	main_fbo->GenerateAttachements(width, height);

	ping_fbo->SetFloatingPoint(hdr);
	ping_fbo->ClearAttachements();
	ping_fbo->GenerateAttachements(width, height);

	pong_fbo->SetFloatingPoint(hdr);
	pong_fbo->ClearAttachements();
	pong_fbo->GenerateAttachements(width, height);
}

void Viewport::SetOutput(ViewportOutput output)
{
	viewport_output = output;
}
