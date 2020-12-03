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
	scene_quad->InitQuadUI();


	framebuffers.emplace_back(scene_fbo = new FrameBuffer(5));

	framebuffers.emplace_back(ping_fbo = new FrameBuffer());
	framebuffers.emplace_back(pong_fbo = new FrameBuffer());
	framebuffers.emplace_back(debug_depth_map_fbo = new FrameBuffer());
	framebuffers.emplace_back(postprocess_fbo = new FrameBuffer());
	framebuffers.emplace_back(blit_fbo = new FrameBuffer());

	depth_full_fbo = new DepthFrameBuffer();
	depth_near_fbo = new DepthFrameBuffer();
	depth_mid_fbo = new DepthFrameBuffer();
	depth_far_fbo = new DepthFrameBuffer();

	SetOutput(ViewportOutput::COLOR);
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
	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_2D, depth_full_fbo->GetColorAttachement());
	glUniform1i(glGetUniformLocation(program, "full_depth_map"), 13);

	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_2D, depth_near_fbo->GetColorAttachement());
	glUniform1i(glGetUniformLocation(program, "close_depth_map"), 14);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, depth_mid_fbo->GetColorAttachement());
	glUniform1i(glGetUniformLocation(program, "mid_depth_map"), 15);

	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, depth_far_fbo->GetColorAttachement());
	glUniform1i(glGetUniformLocation(program, "far_depth_map"), 16);
}

void Viewport::LightCameraPass() const
{
	if (!shadows_pass || App->cameras->main_camera == nullptr)
	{
		return;
	}

	if (!App->renderer->cascade_mapping)
	{
		DepthMapPass(App->lights->full_frustum, depth_full_fbo, App->renderer->depth_map_debug);
	}
	else
	{
		DepthMapPass(App->lights->near_frustum, depth_near_fbo, App->renderer->depth_map_debug && App->renderer->depth_map_debug_index == 0);
		DepthMapPass(App->lights->mid_frustum, depth_mid_fbo, App->renderer->depth_map_debug && App->renderer->depth_map_debug_index == 1);
		DepthMapPass(App->lights->far_frustum, depth_far_fbo, App->renderer->depth_map_debug && App->renderer->depth_map_debug_index == 2);
	}
}

void Viewport::MeshRenderPass() const
{
	scene_fbo->Bind();
	scene_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT0, camera->camera_clear_color);
	scene_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT1);
	scene_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT2);
	scene_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT3);
	scene_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT4);


	if (camera->HasSkybox())
	{
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		SkyboxPass();
	}

	static GLenum attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	num_rendered_triangles = 0;
	num_rendered_vertices = 0;

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

			num_rendered_triangles += opaque_mesh_renderer.mesh_renderer->mesh_to_render->GetNumTriangles();
			num_rendered_vertices += opaque_mesh_renderer.mesh_renderer->mesh_to_render->GetNumVerts();

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

			num_rendered_triangles += transparent_mesh_renderer.mesh_renderer->mesh_to_render->GetNumTriangles();
			num_rendered_vertices += transparent_mesh_renderer.mesh_renderer->mesh_to_render->GetNumVerts();

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

	scene_fbo->Bind();
	App->effects->Render();
	FrameBuffer::UnBind();
}

void Viewport::UIRenderPass() const
{
	if (!IsOptionSet(ViewportOption::RENDER_UI))
	{
		return;
	}

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
	source_fbo->Bind(GL_READ_FRAMEBUFFER);

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

	scene_fbo->Bind();
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

	scene_fbo->Bind();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	App->debug_draw->RenderGrid();
	if (App->debug->show_navmesh)
	{
		App->debug_draw->RenderNavMesh(*camera);
	}
	App->debug_draw->RenderBillboards();
	if (App->editor->selected_game_object != nullptr)
	{
		App->debug_draw->RenderSelectedGameObjectsOutline();
	}
	FrameBuffer::UnBind();
}


void Viewport::SkyboxPass() const
{
	App->cameras->world_skybox->Render(*camera);
}

void Viewport::DepthMapPass(LightFrustum* light_frustum, FrameBuffer* depth_fbo, bool render_debug_depth_map) const
{
	depth_fbo->ClearAttachements();
	depth_fbo->GenerateAttachements(width * light_frustum->multiplier, height * light_frustum->multiplier);

	depth_fbo->Bind();
	BindCameraFrustumMatrices(light_frustum->light_orthogonal_frustum);

	depth_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT0);
	glViewport(0, 0, width * light_frustum->multiplier, height * light_frustum->multiplier);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

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


	if (render_debug_depth_map)
	{
		debug_depth_map_fbo->Bind();
		debug_depth_map_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		GLuint program = App->program->UseProgram("DepthMap");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_fbo->GetColorAttachement());
		glUniform1i(glGetUniformLocation(program, "tex"), 0);

		scene_quad->RenderArray();

		glUseProgram(0);
		FrameBuffer::UnBind();

		depth_map_texture = depth_fbo->GetColorAttachement();
	}
}

void Viewport::BloomPass()
{
	if (!App->renderer->bloom)
	{
		return;
	}

	scene_fbo->Bind(GL_READ_FRAMEBUFFER);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	blit_fbo->Bind(GL_DRAW_FRAMEBUFFER);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	FrameBuffer::UnBind();

	FrameBuffer* current_fbo = ping_fbo;
	FrameBuffer* other_fbo = pong_fbo;

	current_fbo->Bind();
	current_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT0);
	other_fbo->Bind();
	other_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT0);
	FrameBuffer::UnBind();

	bool horizontal = true;
	bool first_iteration = true;

	GLuint shader_program = App->program->UseProgram("Blur", 0);
	for (unsigned int i = 0; i < App->renderer->amount_of_blur; i++)
	{
		current_fbo->Bind();
		current_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glUniform1f(glGetUniformLocation(shader_program, "horizontal"), horizontal);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, first_iteration ? blit_fbo->GetColorAttachement(0) : other_fbo->GetColorAttachement());
		glUniform1i(glGetUniformLocation(shader_program, "image"), 0);

		scene_quad->RenderArray();

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
	postprocess_fbo->ClearColorAttachement(GL_COLOR_ATTACHMENT0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	int shader_variation = 0;
	if (antialiasing)
	{
		shader_variation |= (int)ModuleProgram::ShaderVariation::ENABLE_MSAA;
	}
	if (fog)
	{
		shader_variation |= (int)ModuleProgram::ShaderVariation::ENABLE_FOG;
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
	if (bloom)
	{
		shader_variation |= (int)ModuleProgram::ShaderVariation::ENABLE_BLOOM;
	}
	GLuint program = App->program->UseProgram("PostProcessing", shader_variation);

	if (antialiasing)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, scene_fbo->GetColorAttachement());
		glUniform1i(glGetUniformLocation(program, "screen_texture"), 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, scene_fbo->GetColorAttachement(2));
		glUniform1i(glGetUniformLocation(program, "normal_map"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, scene_fbo->GetColorAttachement(3));
		glUniform1i(glGetUniformLocation(program, "position_map"), 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, scene_fbo->GetColorAttachement(4));
		glUniform1i(glGetUniformLocation(program, "ssr_value_map"), 4);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, scene_fbo->GetColorAttachement());
		glUniform1i(glGetUniformLocation(program, "screen_texture"), 0);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, scene_fbo->GetColorAttachement(2));
		glUniform1i(glGetUniformLocation(program, "normal_map"), 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, scene_fbo->GetColorAttachement(3));
		glUniform1i(glGetUniformLocation(program, "position_map"), 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, scene_fbo->GetColorAttachement(4));
		glUniform1i(glGetUniformLocation(program, "ssr_value_map"), 4);
	}
	glUniform1f(glGetUniformLocation(program, "exposure"), App->renderer->exposure);

	glUniform1f(glGetUniformLocation(program, "z_near"), camera->GetNearDistance());
	glUniform1f(glGetUniformLocation(program, "z_far"), camera->GetFarDistance());

	if (bloom)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ping_pong_fbo->GetColorAttachement());
		glUniform1i(glGetUniformLocation(program, "brightness_texture"), 1);
	}

	if (fog)
	{
		if (antialiasing)
		{
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, scene_fbo->GetDepthAttachement());
			glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
			glUniform1i(glGetUniformLocation(program, "depth_texture"), 5);
		}
		else
		{
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, scene_fbo->GetDepthAttachement());
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
			glUniform1i(glGetUniformLocation(program, "depth_texture"), 5);
		}
		glUniform4fv(glGetUniformLocation(program, "fog_color"), 1, App->renderer->fog_color.ptr());
		glUniform1f(glGetUniformLocation(program, "fog_density"), App->renderer->fog_density);
	}

	scene_quad->RenderArray();

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
	displayed_texture = blit_fbo->GetColorAttachement();
}

bool Viewport::IsOptionSet(ViewportOption option) const
{
	return viewport_options & (int)option;
}

void Viewport::SetAntialiasing(bool antialiasing)
{
	this->antialiasing = antialiasing;
	scene_fbo->SetMultiSampled(antialiasing);
	scene_fbo->ClearAttachements();
	scene_fbo->GenerateAttachements(width, height);
}

void Viewport::SetHDR(bool hdr)
{
	this->hdr = hdr;
}

void Viewport::SetBloom(bool bloom)
{
	this->bloom = bloom;
}

void Viewport::SetFog(bool fog)
{
	this->fog = fog;
}

void Viewport::SetOutput(ViewportOutput output)
{
	viewport_output = output;
	switch (viewport_output)
	{
	case Viewport::ViewportOutput::COLOR:
		source_fbo = postprocess_fbo;
		break;

	case Viewport::ViewportOutput::BRIGHTNESS:
		source_fbo = ping_pong_fbo;
		break;

	default:
		source_fbo = nullptr;
		break;
	}
}
