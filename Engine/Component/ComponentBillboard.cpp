#include "ComponentBillboard.h"

#include "Main/Application.h"

#include "Module/ModuleEffects.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

ComponentBillboard::ComponentBillboard() : Component(nullptr, ComponentType::BILLBOARD)
{
	InitData();
}
ComponentBillboard::ComponentBillboard(GameObject* owner) : Component(owner, ComponentType::BILLBOARD)
{
	InitData();
}

ComponentBillboard::~ComponentBillboard()
{
	if (vbo != 0)
	{
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		glDeleteVertexArrays(1, &vao);
	}
}

void ComponentBillboard::InitData()
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ComponentBillboard::SwitchFrame()
{
	time_since_start += App->time->delta_time;
	if (play)
	{
		if (time_since_start * sheet_speed >= 1000)
		{
			current_sprite_x++;
			if (play_once)
			{
				if (current_sprite_y == y_tiles && current_sprite_x == x_tiles-1)
				{
					play = false;
					play_once = false;
					Disable();
					return;
				}
			}
			
			if ((int)current_sprite_x >= x_tiles)
			{
				current_sprite_y--;
				current_sprite_x = 0;
			}

			if ((int)current_sprite_y <= 0)
			{
				current_sprite_y = y_tiles;
			}
			
			time_since_start = 0.f;
		}
	}

}

void ComponentBillboard::ChangeBillboardType(ComponentBillboard::AlignmentType _alignment_type)
{
	alignment_type = _alignment_type;

	if (alignment_type == SPRITESHEET)
		is_spritesheet = true;
	else
		is_spritesheet = false;
}

void ComponentBillboard::EmitOnce()
{
	Enable();
	play_once = true;
	play = true;
	current_sprite_x = 0;
	current_sprite_y = y_tiles - 1;
}

bool ComponentBillboard::IsPlaying()
{
	return play;
}

void ComponentBillboard::Render(const float3& position)
{
	if(!active)
	{
		return;
	}

	GLuint shader_program = App->program->GetShaderProgramId("Billboard");
	glUseProgram(shader_program);

	int subroutine_position;
	glGetProgramStageiv(shader_program, GL_VERTEX_SHADER, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &subroutine_position);
	unsigned* subroutines_indices = new unsigned[subroutine_position];

	//Subroutine functions
	GLuint viewpoint_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "view_point_alignment");
	GLuint crossed_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "crossed_alignment");
	GLuint axial_subroutine = glGetSubroutineIndex(shader_program, GL_VERTEX_SHADER, "axial_alignment");

	//Subroutine uniform
	int selector = glGetSubroutineUniformLocation(shader_program, GL_VERTEX_SHADER, "alignment_selector");

	switch (alignment_type) 
	{
	case VIEW_POINT:
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, subroutine_position, &viewpoint_subroutine);
		break;
	case CROSSED:
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, subroutine_position, &crossed_subroutine);
		break;

	case AXIAL:
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, subroutine_position, &axial_subroutine);
		break;

	case SPRITESHEET:
		if(oriented_to_camera)
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, subroutine_position, &viewpoint_subroutine);

		else
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, subroutine_position, &crossed_subroutine);

		glUniform1i(glGetUniformLocation(shader_program, "billboard.XTiles"), x_tiles);
		glUniform1i(glGetUniformLocation(shader_program, "billboard.YTiles"), y_tiles);
		glUniform1f(glGetUniformLocation(shader_program, "X"), current_sprite_x);
		glUniform1f(glGetUniformLocation(shader_program, "Y"), current_sprite_y);

		glUniform1f(glGetUniformLocation(shader_program, "billboard.speed"), sheet_speed);
		SwitchFrame();
		break;

	default:
		// viewpoint by default, the most consistent one
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, subroutine_position, &viewpoint_subroutine);
		break;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, billboard_texture->opengl_texture);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.texture"), 0);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.width"), width);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.height"), height);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.isSpritesheet"), is_spritesheet);
	glUniform4fv(glGetUniformLocation(shader_program, "billboard.color"),1, (float*)color);
	glUniform3fv(glGetUniformLocation(shader_program, "billboard.center_pos"), 1, position.ptr());


	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
	delete[] subroutines_indices;
}

Component* ComponentBillboard::Clone(bool original_prefab) const
{
	ComponentBillboard* created_component;
	if (original_prefab)
	{
		created_component = new ComponentBillboard();
	}
	else
	{
		created_component = App->effects->CreateComponentBillboard();
	}
	*created_component = *this;
	return created_component;
};

void ComponentBillboard::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentBillboard*>(component_to_copy) = *this;
}


void ComponentBillboard::Delete()
{
	App->effects->RemoveComponentBillboard(this);
}

void ComponentBillboard::SpecializedSave(Config& config) const
{
	config.AddFloat(sheet_speed, "SheetSpeed");
	config.AddInt(static_cast<int>(alignment_type), "BillboardType");
	config.AddUInt(texture_uuid, "TextureUUID");
	config.AddFloat(width, "Width");
	config.AddFloat(height, "Height");
	config.AddInt((unsigned int)x_tiles, "Rows");
	config.AddInt((unsigned int)y_tiles, "Columns");
}

void ComponentBillboard::SpecializedLoad(const Config& config)
{
	
	sheet_speed = config.GetFloat("SheetSpeed", 1.f);
	alignment_type = static_cast<AlignmentType>(config.GetInt("BillboardType", static_cast<int>(AlignmentType::SPRITESHEET)));
	ChangeBillboardType(alignment_type);
	texture_uuid = config.GetUInt("TextureUUID", 0);
	
	ChangeTexture(texture_uuid);

	width = config.GetFloat("Width", 1.f);
	height = config.GetFloat("Height", 1.f);
	x_tiles = config.GetInt("Rows", 1.f);
	y_tiles = config.GetInt("Columns", 1.f);
}

void ComponentBillboard::ChangeTexture(uint32_t texture_uuid)
{
	if (texture_uuid != 0)
	{
		this->texture_uuid = texture_uuid;
		billboard_texture = App->resources->Load<Texture>(texture_uuid);
	}
}


