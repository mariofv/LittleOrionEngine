#include "ComponentTrailRenderer.h"

#include "ComponentBillboard.h"

#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

namespace { const float MAX_TRAIL_VERTICES = 10; } //arbitrary number 

ComponentTrailRenderer::ComponentTrailRenderer() : Component(nullptr, ComponentType::TRAILRENDERER)
{
	InitData();
}

ComponentTrailRenderer::ComponentTrailRenderer(GameObject * _owner) : Component(owner, ComponentType::TRAILRENDERER)
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
}

void ComponentTrailRenderer::InitData()
{
	ChangeTexture(static_cast<uint32_t>(CoreResource::BILLBOARD_DEFAULT_TEXTURE));


	
	unsigned int indices[6] =
	{
		0, 1, 3,
		1, 2, 3
	};



	glGenVertexArrays(1, &trail_vao);
	glGenBuffers(1, &trail_vbo);

	//glBindVertexArray(vao);
	glBindVertexArray(trail_vao);

	glBindBuffer(GL_ARRAY_BUFFER, trail_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_TRAIL_VERTICES * 4 * 5, nullptr, GL_DYNAMIC_DRAW); //3 float position, 2 float color //before it was MAX_VERTICES * 4 *5
	
	
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);*/

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ComponentTrailRenderer::SwitchFrame()
{
	time_since_start += App->time->delta_time;
	APP_LOG_INFO("%.1f", time_since_start);

	if (time_since_start * sheet_speed >= 1)
	{
		current_sprite_x += 1;

		if ((int)current_sprite_x >= x_tiles) {
			current_sprite_y--;
			current_sprite_x = 0;
		}

		if ((int)current_sprite_y <= 0) {
			current_sprite_y = y_tiles;
		}
		time_since_start = 0.f;
	}


}

void ComponentTrailRenderer::Render(float** to_render, int size)
{
	GLuint shader_program = App->program->GetShaderProgramId("Trail");

	glUseProgram(shader_program);

	glBindVertexArray(trail_vao);

	//use glBufferMap to obtain a pointer to buffer data
	glBindBuffer(GL_ARRAY_BUFFER, trail_vbo);
	trail_renderer_vertices = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * MAX_TRAIL_VERTICES * 4 * 5, GL_MAP_WRITE_BIT);// 6 indices
	memcpy(trail_renderer_vertices, *to_render, size);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, trail_texture->opengl_texture);

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), owner->transform.GetGlobalModelMatrix().Transposed().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUniform4fv(glGetUniformLocation(shader_program, "color"), 1, (float*)color);
	//glDrawElements(GL_TRIANGLE_STRIP, rendered_vertices, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, rendered_vertices);
	glBindVertexArray(0);

	glUseProgram(0);
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
		created_component = App->renderer->CreateComponentTrailRenderer(owner);
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
	App->renderer->RemoveComponentTrailRenderer(this);
}

void ComponentTrailRenderer::SpecializedSave(Config& config) const
{
	config.AddFloat(sheet_speed, "SheetSpeed");
	config.AddUInt(texture_uuid, "TextureUUID");
	config.AddFloat(width, "Width");
	config.AddFloat(height, "Height");
	config.AddInt((unsigned int)x_tiles, "Rows");
	config.AddInt((unsigned int)y_tiles, "Columns");
}

void ComponentTrailRenderer::SpecializedLoad(const Config& config)
{

	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	sheet_speed = config.GetFloat("SheetSpeed", 1.f);
	texture_uuid = config.GetUInt("TextureUUID", 0);

	ChangeTexture(texture_uuid);

	width = config.GetFloat("Width", 1.f);
	height = config.GetFloat("Height", 1.f);
	x_tiles = config.GetInt("Rows", 1.f);
	y_tiles = config.GetInt("Columns", 1.f);
}

void ComponentTrailRenderer::ChangeTexture(uint32_t texture_uuid)
{
	if (texture_uuid != 0)
	{
		this->texture_uuid = texture_uuid;
		trail_texture = App->resources->Load<Texture>(texture_uuid);
	}
}


