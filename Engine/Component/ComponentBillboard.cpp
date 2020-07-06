#include "ComponentBillboard.h"

#include "ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleCamera.h"
#include "Module/ModuleEffects.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

ComponentBillboard::ComponentBillboard() : Component(nullptr, ComponentType::BILLBOARD)
{
	InitData();
	ChangeBillboardType(AlignmentType::VIEW_POINT);
}
ComponentBillboard::ComponentBillboard(GameObject* owner) : Component(owner, ComponentType::BILLBOARD)
{
	InitData();
	ChangeBillboardType(AlignmentType::VIEW_POINT);
}

ComponentBillboard::~ComponentBillboard()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
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
	if (play)
	{
		time_since_start += App->time->delta_time;
		if (time_since_start > loop_time)
		{
			if (play_once)
			{
				time_since_start = 0.f;
				play = false;
			}
			else
			{
				time_since_start = time_since_start % loop_time;
			}
		}
		float loop_progress = time_since_start / loop_time;

		int current_sprite = math::FloorInt(loop_progress * num_sprites);
		current_sprite_x = current_sprite / num_sprisheet_columns;
		current_sprite_y = current_sprite % num_sprisheet_columns;
	}
}

void ComponentBillboard::ChangeBillboardType(ComponentBillboard::AlignmentType alignment_type)
{
	this->alignment_type = alignment_type;
}

void ComponentBillboard::EmitOnce()
{
	play = true;
	play_once = true;
	current_sprite_x = 0;
	current_sprite_y = 0;
}

bool ComponentBillboard::IsPlaying()
{
	return play;
}

void ComponentBillboard::Render(const float3& global_position)
{
	if(!active)
	{
		return;
	}
	BROFILER_CATEGORY("Render billboard", Profiler::Color::Orange);

	unsigned int variation = GetBillboardVariation();
	shader_program = App->program->UseProgram("Billboard", variation);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, billboard_texture->opengl_texture);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.texture"), 0);
	glUniform4fv(glGetUniformLocation(shader_program, "billboard.color"),1, (float*)color);

	float4x4 model_matrix = float4x4::FromTRS(global_position, GetSpriteRotation(global_position), float3(width, height, 1.f));
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), model_matrix.Transposed().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
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
	config.AddUInt(texture_uuid, "TextureUUID");
	config.AddFloat(width, "Width");
	config.AddFloat(height, "Height");

	config.AddInt(static_cast<int>(alignment_type), "BillboardType");

	config.AddInt(loop_time, "LoopTime");
	config.AddInt(num_sprisheet_rows, "Rows");
	config.AddInt(num_sprisheet_columns, "Columns");
}

void ComponentBillboard::SpecializedLoad(const Config& config)
{
	texture_uuid = config.GetUInt32("TextureUUID", 0);
	ChangeTexture(texture_uuid);
	
	width = config.GetFloat("Width", 1.0f);
	height = config.GetFloat("Height", 1.0f);

	alignment_type = static_cast<AlignmentType>(config.GetInt("BillboardType", static_cast<int>(AlignmentType::VIEW_POINT)));
	ChangeBillboardType(alignment_type);

	loop_time = config.GetInt("LoopTime", 0);
	num_sprisheet_rows = config.GetInt("Rows", 1);
	num_sprisheet_columns = config.GetInt("Columns", 1);
}

void ComponentBillboard::ChangeTexture(uint32_t texture_uuid)
{
	if (texture_uuid != 0)
	{
		this->texture_uuid = texture_uuid;
		billboard_texture = App->resources->Load<Texture>(texture_uuid);
	}
}

float4x4 ComponentBillboard::GetSpriteRotation(const float3& position) const
{
	float3 up;
	float3 front;
	float3 right;

	float3 camera_position = App->cameras->main_camera->owner->transform.GetGlobalTranslation();

	switch (alignment_type)
	{
	case WORLD:
		up = float3::unitY;
		front = float3::unitZ;
		right = float3::unitX;
		break;

	case VIEW_POINT:
		up = float3::unitY;
		front = (camera_position - position).Normalized();
		right = up.Cross(right);
		break;

	case AXIAL:
		up = float3::unitY;
		right = (camera_position - position).Normalized().Cross(up);
		front = right.Cross(up);
		break;
	}

	float4x4 rotation = float4x4::identity;
	rotation.SetCol3(0, right);
	rotation.SetCol3(1, front);
	rotation.SetCol3(2, up);

	return rotation;
}

unsigned int ComponentBillboard::GetBillboardVariation()
{
	unsigned int variation = 0;

	if (is_spritesheet)
	{
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_SPRITESHEET);
	}

	return variation;
}