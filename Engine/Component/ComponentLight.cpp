#include "ComponentLight.h"
#include "Application.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleProgram.h"
#include "UI/ComponentsUI.h"
#include "GameObject.h"

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

void ComponentLight::Render(unsigned int shader_program) const
{
	//TODO: Modify this once uniform buffer is created
	size_t size_of_elements_before = 3 * sizeof(float3);
	/*glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniforms_buffer);
	glBufferSubData(GL_UNIFORM_BUFFER, size_of_elements_before, 3* sizeof(float), light_color);
	glBufferSubData(GL_UNIFORM_BUFFER, size_of_elements_before + 3 * sizeof(float), sizeof(float), &light_intensity);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);*/

	
	glUniform1f(glGetUniformLocation(shader_program, "light.light_intensity"), light_intensity);
	glUniform3fv(glGetUniformLocation(shader_program, "light.light_color"), 1, (float*)light_color);
	glUniform3fv(glGetUniformLocation(shader_program, "light.light_position"), 1, (float*)&owner->transform.GetTranslation());
	
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