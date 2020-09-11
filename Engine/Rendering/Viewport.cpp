#include "Viewport.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"

#include "Main/Application.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleSpacePartitioning.h"

#include "FrameBuffer.h"

Viewport::Viewport()
{
	render_fbo = new FrameBuffer();
	debug_draw_fbo = new FrameBuffer();
}

Viewport::~Viewport()
{
	delete render_fbo;
	delete debug_draw_fbo;
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

	render_fbo->ClearAttachements();
	render_fbo->GenerateAttachements(width, height);
}

void Viewport::Render(ComponentCamera* camera)
{
	this->camera = camera;
	camera->SetAspectRatio(width / height);
	BindCameraMatrices();

	MeshRenderPass();
	DebugDrawPass();
	EditorDrawPass();

	last_displayed_texture = render_fbo->GetColorAttachement();
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
	render_fbo->Bind();
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

	render_fbo->UnBind();
}

void Viewport::DebugDrawPass() const
{
	render_fbo->Bind();
	App->debug_draw->Render(width, height, camera->GetProjectionMatrix() * camera->GetViewMatrix());
	render_fbo->UnBind();
}

void Viewport::EditorDrawPass() const
{
	render_fbo->Bind();
	App->debug_draw->RenderGrid();
	if (App->debug->show_navmesh)
	{
		App->debug_draw->RenderNavMesh(*camera);
	}
	App->debug_draw->RenderBillboards();
	if (App->editor->selected_game_object != nullptr)
	{
		App->debug_draw->RenderOutline(); // This function tries to render again the selected game object. It will fail because depth buffer
	}
	render_fbo->UnBind();
}