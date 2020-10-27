#include "ComponentBillboard.h"

#include "ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleEffects.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleTexture.h"
#include "Rendering/Viewport.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

ComponentBillboard::ComponentBillboard() : Component(nullptr, ComponentType::BILLBOARD)
{
	InitData();
}
ComponentBillboard::ComponentBillboard(GameObject* owner) : Component(owner, ComponentType::BILLBOARD)
{
	InitData();
}

void ComponentBillboard::InitData()
{
	ChangeTexture(texture_uuid);
	ChangeTextureEmissive(texture_emissive_uuid);
	quad = App->effects->quad.get();
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
		if (pulse)
		{
			emissive_intensity = sin(time_since_start);
		}
	}
}

void ComponentBillboard::ChangeBillboardType(ComponentBillboard::AlignmentType alignment_type)
{
	this->alignment_type = alignment_type;
}

void ComponentBillboard::ComputeAnimationFrame(float progress)
{
	num_sprites = num_sprisheet_columns * num_sprisheet_rows;

	int current_sprite = min(math::FloorInt(progress * num_sprites), num_sprites - 1);
	current_sprite_x = current_sprite % num_sprisheet_columns;
	current_sprite_y =  (num_sprisheet_rows - 1) - math::FloorInt(current_sprite / num_sprisheet_columns);
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
	BROFILER_CATEGORY("Render billboard", Profiler::Color::Orange);
	if(!active || !billboard_texture || !billboard_texture->initialized)
	{
		return;
	}
	BROFILER_CATEGORY("Render billboard", Profiler::Color::Orange);

	unsigned int variation = GetBillboardVariation();
	shader_program = App->program->UseProgram("Billboard", variation);

	CommonUniforms(shader_program);
	glUniform4fv(glGetUniformLocation(shader_program, "billboard.color"), 1, color.ptr());
	glUniform4fv(glGetUniformLocation(shader_program, "billboard.color_emissive"), 1, color_emissive.ptr());

	glUniform1i(glGetUniformLocation(shader_program, "billboard.current_sprite_x"), current_sprite_x);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.current_sprite_y"), current_sprite_y);


	float4x4 model_matrix = float4x4::FromTRS(global_position, owner->transform.GetGlobalRotation(), float3(width, height, 1.f));
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.MATRICES_UNIFORMS_OFFSET, sizeof(float4x4), model_matrix.Transposed().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	quad->RenderElement();

	glUseProgram(0);
}

void ComponentBillboard::CommonUniforms(const GLuint &shader_program)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, billboard_texture->opengl_texture);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.texture"), 0);

	glActiveTexture(GL_TEXTURE1);
	GLuint emissive_texture_id;
	if (texture_emissive_uuid == 0)
	{
		emissive_texture_id = App->texture->blackfall_texture_id;
	}
	else
	{
		emissive_texture_id = billboard_texture_emissive->opengl_texture;
	}
	glBindTexture(GL_TEXTURE_2D, emissive_texture_id);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.texture_emissive"), 1);
	glUniform1f(glGetUniformLocation(shader_program, "billboard.emissive_intensity"), emissive_intensity);

	glUniform1i(glGetUniformLocation(shader_program, "billboard.num_rows"), num_sprisheet_rows);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.num_columns"), num_sprisheet_columns);
	glUniform1i(glGetUniformLocation(shader_program, "billboard.num_columns"), num_sprisheet_columns);
}

Component* ComponentBillboard::Clone(GameObject* owner, bool original_prefab)
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
	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
	return created_component;
}

void ComponentBillboard::CopyTo(Component* component_to_copy) const
{
	ComponentBillboard* billboard = static_cast<ComponentBillboard*>(component_to_copy);
	*billboard = *this;
}


void ComponentBillboard::Delete()
{
	App->effects->RemoveComponentBillboard(this);
}

void ComponentBillboard::SpecializedSave(Config& config) const
{
	config.AddUInt(texture_uuid, "TextureUUID");
	config.AddUInt(texture_emissive_uuid, "TextureEmissiveUUID");
	config.AddFloat(width, "Width");
	config.AddFloat(height, "Height");
	config.AddBool( pulse, "Pulse");
	config.AddBool(loop, "Loop");
	config.AddInt(static_cast<int>(alignment_type), "BillboardType");

	config.AddInt(animation_time, "AnimationTime");
	config.AddBool(is_spritesheet, "IsSpriteSheet");
	config.AddInt(num_sprisheet_rows, "Rows");
	config.AddInt(num_sprisheet_columns, "Columns");

	float4 billbaord_color(color[0], color[1], color[2], color[3]);
	config.AddColor(billbaord_color, "Color");

	float4 billbaord_color_emissive(color_emissive[0], color_emissive[1], color_emissive[2], color_emissive[3]);
	config.AddColor(billbaord_color_emissive, "ColorEmissive");
	config.AddFloat(emissive_intensity, "Emissive Intensity");
	config.AddBool(playing_once, "Playing Once");
}

void ComponentBillboard::SpecializedLoad(const Config& config)
{
	texture_uuid = config.GetUInt32("TextureUUID", 0);
	ChangeTexture(texture_uuid);

	texture_emissive_uuid = config.GetUInt32("TextureEmissiveUUID", 0);
	ChangeTextureEmissive(texture_emissive_uuid);

	width = config.GetFloat("Width", 1.0f);
	height = config.GetFloat("Height", 1.0f);
	pulse = config.GetBool("Pulse", false);
	loop = config.GetBool("Loop", false);
	playing_once = config.GetBool("Playing Once", false);
	alignment_type = static_cast<AlignmentType>(config.GetInt("BillboardType", static_cast<int>(AlignmentType::WORLD)));
	ChangeBillboardType(alignment_type);

	animation_time = config.GetInt("AnimationTime", 1000);
	is_spritesheet = config.GetBool("IsSpriteSheet", false);
	num_sprisheet_rows = config.GetInt("Rows", 1);
	num_sprisheet_columns = config.GetInt("Columns", 1);

	float4 billbaord_color;
	config.GetColor("Color", billbaord_color, float4(1.0f, 1.0f, 1.0f, 1.0f));

	color[0] = billbaord_color.x;
	color[1] = billbaord_color.y;
	color[2] = billbaord_color.z;
	color[3] = billbaord_color.w;

	float4 billbaord_color_emissive;
	config.GetColor("ColorEmissive", billbaord_color_emissive, float4(1.0f, 1.0f, 1.0f, 1.0f));

	color_emissive[0] = billbaord_color_emissive.x;
	color_emissive[1] = billbaord_color_emissive.y;
	color_emissive[2] = billbaord_color_emissive.z;
	color_emissive[3] = billbaord_color_emissive.w;

	emissive_intensity = config.GetFloat("Emissive Intensity", 1.f);
}

void ComponentBillboard::ChangeTexture(uint32_t texture_uuid)
{
	if (texture_uuid != 0)
	{
		//Prepare multithreading loading
		App->resources->loading_thread_communication.current_component_loading = this;
		App->resources->loading_thread_communication.current_type = ResourceType::TEXTURE;
		this->texture_uuid = texture_uuid;
		billboard_texture = App->resources->Load<Texture>(texture_uuid);

		//Set to default loading component
		App->resources->loading_thread_communication.current_component_loading = nullptr;
	}
}


void ComponentBillboard::LoadResource(uint32_t uuid, ResourceType resource)
{
	billboard_texture = std::static_pointer_cast<Texture>(App->resources->RetrieveFromCacheIfExist(uuid));

	if (billboard_texture)
	{
		return;
	}

	FileData file_data;
	bool succes = App->resources->RetrieveFileDataByUUID(uuid, file_data);
	if (succes)
	{
		//THINK WHAT TO DO IF IS IN CACHE
		billboard_texture = ResourceManagement::Load<Texture>(uuid, file_data, true);
		//Delete file data buffer
		delete[] file_data.buffer;
		App->resources->AddResourceToCache(std::static_pointer_cast<Resource>(billboard_texture));
	}

}

void ComponentBillboard::InitResource(uint32_t uuid, ResourceType resource)
{
	if (billboard_texture && !billboard_texture.get()->initialized)
	{
		billboard_texture.get()->LoadInMemory();
	}
}

void ComponentBillboard::ReassignResource()
{
	ChangeTexture(texture_uuid);
}

void ComponentBillboard::ChangeTextureEmissive(uint32_t texture_uuid)
{
	App->resources->loading_thread_communication.normal_loading_flag = true;

	if (texture_uuid != 0)
	{
		this->texture_emissive_uuid = texture_uuid;
		billboard_texture_emissive = App->resources->Load<Texture>(texture_uuid);
	}
	App->resources->loading_thread_communication.normal_loading_flag = false;
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

void ComponentBillboard::SetOrientation(bool is_oriented)
{
	oriented_to_camera = is_oriented;
}

ENGINE_API void ComponentBillboard::SetAnimationTime(size_t time)
{
	animation_time = time;
}

bool ComponentBillboard::HasToDrawBillboard() const
{
	if (App->time->isGameRunning() || App->renderer->game_viewport->effects_draw_all)
	{
		return owner->IsEnabled();
	}

	if (App->editor->selected_game_object == nullptr)
	{
		return false;
	}

	if (owner->IsEnabled())
	{
		return App->editor->selected_game_object->UUID == owner->UUID;
	}

	return false;
}

void ComponentBillboard::Disable()
{
	active = false;
	playing = false;
}
