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
}
ComponentBillboard::ComponentBillboard(GameObject* owner) : Component(owner, ComponentType::BILLBOARD)
{
	InitData();
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

void ComponentBillboard::Update()
{
	if (playing)
	{
		time_since_start += App->time->delta_time;
		if (time_since_start > animation_time)
		{
			if (!loop)
			{
				time_since_start = 0.f;
				playing = false;
				active = false;
			}
			else
			{
				time_since_start = time_since_start % animation_time;
			}
		}
		float loop_progress = (float)time_since_start / animation_time;

		ComputeAnimationFrame(loop_progress);
	}
}

void ComponentBillboard::ChangeBillboardType(ComponentBillboard::AlignmentType alignment_type)
{
	this->alignment_type = alignment_type;
}

void ComponentBillboard::ComputeAnimationFrame(float progress)
{
	num_sprites = num_sprisheet_columns * num_sprisheet_rows;

	int current_sprite = math::FloorInt(progress * num_sprites);
	current_sprite_x = current_sprite % num_sprisheet_columns;
	current_sprite_y = (num_sprisheet_columns - 1) - current_sprite / num_sprisheet_columns;
}

void ComponentBillboard::Play()
{
	if (!active)
	{
		active = true;
	}
	playing = true;
	time_since_start = 0;
	current_sprite_x = 0;
	current_sprite_y = 0;
}

bool ComponentBillboard::IsPlaying()
{
	return playing;
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

	CommonUniforms(shader_program);
	glUniform4fv(glGetUniformLocation(shader_program, "billboard.color"),1, (float*)color);

	glUniform1i(glGetUniformLocation(shader_program, "billboard.current_sprite_x"), current_sprite_x);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.current_sprite_y"), current_sprite_y);

	float4x4 model_matrix = float4x4::FromTRS(global_position, owner->transform.GetGlobalRotation(), float3(width, height, 1.f));
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), model_matrix.Transposed().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glUseProgram(0);
}

void ComponentBillboard::CommonUniforms(const GLuint &shader_program)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, billboard_texture->opengl_texture);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.texture"), 0);

	glUniform1i(glGetUniformLocation(shader_program, "billboard.num_rows"), num_sprisheet_rows);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.num_columns"), num_sprisheet_columns);
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

	config.AddInt(animation_time, "AnimationTime");
	config.AddBool(is_spritesheet, "IsSpriteSheet");
	config.AddInt(num_sprisheet_rows, "Rows");
	config.AddInt(num_sprisheet_columns, "Columns");
}

void ComponentBillboard::SpecializedLoad(const Config& config)
{
	texture_uuid = config.GetUInt32("TextureUUID", 0);
	ChangeTexture(texture_uuid);
	
	width = config.GetFloat("Width", 1.0f);
	height = config.GetFloat("Height", 1.0f);

	alignment_type = static_cast<AlignmentType>(config.GetInt("BillboardType", static_cast<int>(AlignmentType::WORLD)));
	ChangeBillboardType(alignment_type);

	animation_time = config.GetInt("AnimationTime", 1000);
	is_spritesheet = config.GetBool("IsSpriteSheet", false);
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

unsigned int ComponentBillboard::GetBillboardVariation()
{
	unsigned int variation = 0;

	switch (alignment_type)
	{
	case VIEW_POINT:
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_BILLBOARD_VIEWPOINT_ALIGNMENT);
		break;

	case AXIAL:
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_BILLBOARD_AXIAL_ALIGNMENT);
		break;
	}

	if (is_spritesheet)
	{
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_SPRITESHEET);
	}

	return variation;
}