#include "ComponentLight.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"

ComponentLight::ComponentLight() : Component(nullptr, ComponentType::LIGHT)
{
}

ComponentLight::ComponentLight(GameObject * owner) : Component(owner, ComponentType::LIGHT)
{

}

void ComponentLight::Delete()
{
	App->lights->RemoveComponentLight(this);
}

/*
<<<<<<< HEAD
void ComponentLight::Render() const
{	

	float3 light_color_scaled = light_intensity * float3(light_color);

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.lights_uniform_offset, 3 * sizeof(float), light_color_scaled.ptr());

	size_t light_direction_offset = App->program->uniform_buffer.lights_uniform_offset + 4 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_direction_offset, 3 * sizeof(float), owner->transform.GetFrontVector().ptr());

	size_t light_position_offset = App->program->uniform_buffer.lights_uniform_offset + 8 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_position_offset, 3 * sizeof(float), owner->transform.GetGlobalTranslation().ptr());

	float cutOff_value = cos((12.5f*3,1415)/180);
	size_t light_cutOff_offset = App->program->uniform_buffer.lights_uniform_offset + 11 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_cutOff_offset, sizeof(float), &cutOff_value);

	float outerCutOff_value = cos((17.5f * 3, 1415) / 180);
	size_t light_outerCutOff_offset = App->program->uniform_buffer.lights_uniform_offset + 12 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_outerCutOff_offset, sizeof(float), &outerCutOff_value);

	float constant_value = 1.0F;
	size_t light_constant_offset = App->program->uniform_buffer.lights_uniform_offset + 13 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_constant_offset, sizeof(float), &constant_value);

	float linear_value = 0.09F;
	size_t light_linear_offset = App->program->uniform_buffer.lights_uniform_offset + 14 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_linear_offset, sizeof(float), &linear_value);

	float quadratic_value = 0.032F;
	size_t light_quadratic_offset = App->program->uniform_buffer.lights_uniform_offset + 15 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_quadratic_offset, sizeof(float), &quadratic_value);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
*/
void ComponentLight::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddInt((unsigned int)type, "ComponentType");
	config.AddBool(active, "Active");
	config.AddColor(float4(light_color[0], light_color[1], light_color[2], 1.f), "LightColor");
	config.AddFloat(light_intensity, "Intensity");
	
}

void ComponentLight::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

	float4 color;
	config.GetColor("LightColor", color, float4(0.f, 0.f, 0.f, 1.f));
	light_color[0] = color.x;
	light_color[1] = color.y;
	light_color[2] = color.z;

	light_intensity = config.GetFloat("Intensity", 1.f);

}

int ComponentLight::GetLightTypeInt() const
{
	switch (light_type)
	{
	case LightType::POINT_LIGHT:
		return 0;
	case LightType::SPOT_LIGHT:
		return 1;
	case LightType::DIRECTIONAL_LIGHT:
		return 2;
	}
}