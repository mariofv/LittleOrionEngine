#include "ModuleLight.h"
#include "Main/Application.h"
#include "Component/ComponentLight.h"
#include "Module/ModuleProgram.h"
#include "Main/GameObject.h"

#include <Brofiler/Brofiler.h>

ModuleLight::~ModuleLight()
{
	CleanUp();
}

bool ModuleLight::Init()
{
	APP_LOG_SECTION("************ Module Light Init ************");

	/*
	light_positions = std::vector<float4>(MAX_LIGHTS_RENDERED);
	light_colors = std::vector<float4>(MAX_LIGHTS_RENDERED);
	*/

	return true;
}

bool ModuleLight::CleanUp()
{
	for (auto& light : lights)
	{
		light->owner->RemoveComponent(light);
	}
	lights.clear();
	return true;
}

void ModuleLight::Render()
{
	RenderDirectionalLight();


	/*
	BROFILER_CATEGORY("Render Lights", Profiler::Color::White);
	for (auto& light : lights)
	{
		if (light->IsEnabled())
		{
			switch (light->light_type)
			{
			case ComponentLight::LightType::POINT_LIGHT:
			{
				light_positions[current_lights_rendered] = float4(light->owner->transform.GetGlobalTranslation(), 1.f);
				light_colors[current_lights_rendered] = light->light_intensity * float4(float3(light->light_color), 1.f);

				light_colors[current_lights_rendered] = light->light_intensity * float4(float3(light->light_color), 1.f);  // constant

				light_colors[current_lights_rendered] = light->light_intensity * float4(float3(light->light_color), 1.f); // linear
				
				light_colors[current_lights_rendered] = light->light_intensity * float4(float3(light->light_color), 1.f); // quadratic
			}

			default:
				break;
			}
			light_positions[current_lights_rendered] = float4(light->owner->transform.GetGlobalTranslation(), 1.f);
			light_colors[current_lights_rendered] = light->light_intensity * float4(float3(light->light_color), 1.f);
			++current_lights_rendered;
		}
	}
	*/
}

void ModuleLight::RenderDirectionalLight()
{
	current_number_directional_lights_rendered = 0;
	for (auto& light : lights)
	{
		if (light->IsEnabled())
		{
			if (light->light_type == ComponentLight::LightType::DIRECTIONAL_LIGHT)
			{
				float3 light_color_scaled = light->light_intensity * float3(light->light_color);

				glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

				glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.lights_uniform_offset, sizeof(float3), light_color_scaled.ptr());

				size_t light_direction_offset = App->program->uniform_buffer.lights_uniform_offset + 4 * sizeof(float);
				glBufferSubData(GL_UNIFORM_BUFFER, light_direction_offset, sizeof(float3), light->owner->transform.GetFrontVector().ptr());

				glBindBuffer(GL_UNIFORM_BUFFER, 0);
			}
		}
	}
}

void ModuleLight::RenderLights()
{
	/*
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.lights_uniform_offset, sizeof(float), &current_lights_rendered);

	size_t light_positions_offset = App->program->uniform_buffer.lights_uniform_offset + 4 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_positions_offset, MAX_LIGHTS_RENDERED * sizeof(float4), &light_positions[0]);

	size_t light_colors_offset = light_positions_offset + MAX_LIGHTS_RENDERED * sizeof(float4);
	glBufferSubData(GL_UNIFORM_BUFFER, light_colors_offset, MAX_LIGHTS_RENDERED * sizeof(float4), &light_colors[0]);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	*/
}

void ModuleLight::RenderDarkness() const
{
	/*
	float3 darkness_color = float3(0.f);
	float3 darkness_position = float3(0.f);

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.lights_uniform_offset, 3 * sizeof(float), darkness_color.ptr());

	size_t light_position_offset = App->program->uniform_buffer.lights_uniform_offset + 4 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_position_offset, 3 * sizeof(float), darkness_position.ptr());

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	*/
}

ComponentLight* ModuleLight::CreateComponentLight()
{
	ComponentLight * created_light = new ComponentLight();
	lights.push_back(created_light);
	return created_light;
}

void ModuleLight::RemoveComponentLight(ComponentLight* light_to_remove)
{
	auto it = std::find(lights.begin(), lights.end(), light_to_remove);
	if (it != lights.end())
	{
		delete *it;
		lights.erase(it);
	}
}