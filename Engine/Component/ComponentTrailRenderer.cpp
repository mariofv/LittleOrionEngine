#include "ComponentTrailRenderer.h"

#include "Main/Application.h"

#include "Module/ModuleEffects.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

namespace { const float MAX_TRAIL_VERTICES = 1000; } //arbitrary number 

ComponentTrailRenderer::ComponentTrailRenderer() : Component(nullptr, ComponentType::TRAILRENDERER)
{
	InitData();
}

ComponentTrailRenderer::ComponentTrailRenderer(GameObject * owner) : Component(owner, ComponentType::TRAILRENDERER)
{
	InitData();
}

ComponentTrailRenderer::~ComponentTrailRenderer()
{
	if (trail_vbo != 0)
	{
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glDeleteBuffers(1, &trail_vbo);
		glDeleteBuffers(1, &trail_vao);
	}
	App->effects->RemoveComponentTrailRenderer(this);
}

void ComponentTrailRenderer::InitData()
{
	ChangeTexture(static_cast<uint32_t>(CoreResource::BILLBOARD_DEFAULT_TEXTURE));

	glGenVertexArrays(1, &trail_vao);
	glGenBuffers(1, &trail_vbo);

	glBindVertexArray(trail_vao);

	glBindBuffer(GL_ARRAY_BUFFER, trail_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_TRAIL_VERTICES * 4 * 5, nullptr, GL_DYNAMIC_DRAW); 
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ComponentTrailRenderer::Render(std::vector<Vertex>& to_render)
{
	if (active)
	{
		
		GLuint shader_program = App->program->UseProgram("Trail");
		glUseProgram(shader_program);

		glBindVertexArray(trail_vao);

		//use glBufferMap to obtain a pointer to buffer data
		glBindBuffer(GL_ARRAY_BUFFER, trail_vbo);
		trail_renderer_vertices = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(Vertex) *  to_render.size(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);// 6 indices
		memcpy(trail_renderer_vertices, to_render.data(), to_render.size() * sizeof(Vertex));
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, trail_texture->opengl_texture);
		glUniform1i(glGetUniformLocation(shader_program, "tex"), 0);

		glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), owner->transform.GetGlobalModelMatrix().Transposed().ptr());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glUniform4fv(glGetUniformLocation(shader_program, "color"), 1, (float*)color);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, to_render.size());
		glBindVertexArray(0);

		glUseProgram(0);
	}
}

Component* ComponentTrailRenderer::Clone(bool original_prefab) const
{
	ComponentTrailRenderer * created_component;
	if (original_prefab)
	{
		created_component = new ComponentTrailRenderer();
	}
	else
	{
		created_component = App->effects->CreateComponentTrailRenderer(owner);
	}
	*created_component = *this;
	return created_component;
};

void ComponentTrailRenderer::Copy(Component * component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentTrailRenderer*>(component_to_copy) = *this;
}


void ComponentTrailRenderer::Delete()
{
	App->effects->RemoveComponentTrailRenderer(this);
}

void ComponentTrailRenderer::SpecializedSave(Config& config) const
{
	config.AddUInt(texture_uuid, "TextureUUID");
}

void ComponentTrailRenderer::SpecializedLoad(const Config& config)
{

	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	texture_uuid = config.GetUInt("TextureUUID", 0);

	ChangeTexture(texture_uuid);

}

void ComponentTrailRenderer::ChangeTexture(uint32_t texture_uuid)
{
	if (texture_uuid != 0)
	{
		this->texture_uuid = texture_uuid;
		trail_texture = App->resources->Load<Texture>(texture_uuid);
	}
}


