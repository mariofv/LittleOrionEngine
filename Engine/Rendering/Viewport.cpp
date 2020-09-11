#include "Viewport.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleSpacePartitioning.h"

#include "FrameBuffer.h"

Viewport::Viewport()
{
	render_fbo = new FrameBuffer();
}

Viewport::~Viewport()
{
	delete render_fbo;
}

void Viewport::SetSize(float width, float height)
{
	if (this->width == width && this->height == height)
	{
		return;
	}

	this->width = width;
	this->height = height;

	render_fbo->ClearAttachements();
	render_fbo->GenerateAttachements(width, height);
}

void Viewport::Render(ComponentCamera* camera)
{
	this->camera = camera;
	camera->SetAspectRatio(width / height);
	BindCameraMatrices();

	render_fbo->Bind();
	glViewport(0, 0, width, height);
	camera->Clear();

	MeshRenderPass();

	render_fbo->UnBind();

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
	std::vector<ComponentMeshRenderer*> culled_mesh_renderers = App->space_partitioning->GetCullingMeshes(camera, App->renderer->mesh_renderers);
	for (auto &mesh_renderer : culled_mesh_renderers)
	{
		if (mesh_renderer->mesh_to_render != nullptr && mesh_renderer->IsEnabled())
		{
			mesh_renderer->Render();
			glUseProgram(0);
		}
	}
}
