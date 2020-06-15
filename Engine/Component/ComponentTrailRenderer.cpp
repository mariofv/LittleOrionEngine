#include "ComponentTrailRenderer.h"

#include "ComponentBillboard.h"

#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

namespace { const float MAX_TRAIL_VERTICES = 5000; } //arbitrary number 

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
	if (vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);
	}
}

void ComponentTrailRenderer::InitData()
{
	ChangeTexture(static_cast<uint32_t>(CoreResource::BILLBOARD_DEFAULT_TEXTURE));

	float vertices[20] =
	{
			0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
			0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
		   -0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
		   -0.5f,  0.5f, 0.0f,		0.0f, 1.0f
	};
	unsigned int indices[6] =
	{
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);//dynamically draw vertices due to trail changes mesh over time

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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

void ComponentTrailRenderer::Render(const float3& position)
{
	GLuint shader_program = App->program->GetShaderProgramId("Billboard");
	glUseProgram(shader_program);

	int n;
	glGetProgramStageiv(shader_program, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &n);
	unsigned* subroutines_indices = new unsigned[n];

	//use glBufferMap to obtain a pointer to buffer data
	trail_renderer_vertices = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(float) * MAX_TRAIL_VERTICES * 6, GL_MAP_WRITE_BIT); 

	//Subroutine functions
	GLuint viewpoint_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "view_point_alignment");
	GLuint crossed_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "crossed_alignment");
	GLuint axial_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "axial_alignment");

	//Subroutine uniform
	int selector = glGetSubroutineUniformLocation(shader_program, GL_VERTEX_SHADER, "alignment_selector");

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, trail_texture->opengl_texture);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.texture"), 0);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.width"), width);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.height"), height);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.isSpritesheet"), is_spritesheet);
	glUniform4fv(glGetUniformLocation(shader_program, "billboard.color"), 1, (float*)color);
	glUniform3fv(glGetUniformLocation(shader_program, "billboard.center_pos"), 1, position.ptr());

	glBindVertexArray(vao);
	 
	glDrawElements(GL_TRIANGLES, rendered_vertices, GL_UNSIGNED_INT, 0);
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


