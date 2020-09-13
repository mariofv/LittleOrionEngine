#include "Viewport.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"

#include "Main/Application.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleEffects.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleSpacePartitioning.h"

#include "FrameBuffer.h"
#include "MultiSampledFrameBuffer.h"

Viewport::Viewport(bool is_scene_viewport) : is_scene_viewport(is_scene_viewport)
{
	regular_fbo = new FrameBuffer();
	multisampled_fbo = new MultiSampledFrameBuffer();

	main_fbo = regular_fbo;
}

Viewport::~Viewport()
{
	delete regular_fbo;
	delete multisampled_fbo;
}

void Viewport::Render(ComponentCamera* camera)
{
	this->camera = camera;
	camera->SetAspectRatio(width / height);
	BindCameraMatrices();

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

void Viewport::MeshRenderPass() const
{
	main_fbo->Bind();
	camera->Clear();

	std::vector<ComponentMeshRenderer*> culled_mesh_renderers = App->space_partitioning->GetCullingMeshes(camera, App->renderer->mesh_renderers);
	for (auto &mesh_renderer : culled_mesh_renderers)
	{
		if (mesh_renderer->mesh_to_render != nullptr && mesh_renderer->IsEnabled())
		{
			mesh_renderer->Render();
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
	App->ui->Render(width, height, is_scene_viewport);
	FrameBuffer::UnBind();
}

void Viewport::PostProcessPass() const
{
	if (antialiasing)
	{
		main_fbo->Bind(GL_READ_FRAMEBUFFER);
		regular_fbo->Bind(GL_DRAW_FRAMEBUFFER);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		FrameBuffer::UnBind();
	}

	//App->renderer->RenderPostProcessingEffects(*camera);
}

void Viewport::DebugPass() const
{
	if (!debug_pass || !is_scene_viewport)
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
	if (!is_scene_viewport)
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

	regular_fbo->ClearAttachements();
	regular_fbo->GenerateAttachements(width, height);

	multisampled_fbo->ClearAttachements();
	multisampled_fbo->GenerateAttachements(width, height);
}

void Viewport::SelectLastDisplayedTexture()
{
	if (antialiasing)
	{
		last_displayed_texture = regular_fbo->GetColorAttachement();
	}
	else
	{
		last_displayed_texture = main_fbo->GetColorAttachement();
	}
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
