#include "ComponentLight.h"
#include "Main/Application.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"
#include "UI/ComponentsUI.h"
#include "Main/GameObject.h"

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

void ComponentLight::Render() const
{	
	float3 light_color_scaled = light_intensity * float3(light_color);

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	
	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.lights_uniform_offset, 3 * sizeof(float), light_color_scaled.ptr());

	size_t light_position_offset = App->program->uniform_buffer.lights_uniform_offset + 4 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_position_offset, 3 * sizeof(float), owner->transform.GetGlobalTranslation().ptr());

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

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

void ComponentLight::ShowComponentWindow()
{
	ComponentsUI::ShowComponentLightWindow(this);
}