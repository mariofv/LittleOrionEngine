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

void ModuleLight::Render(GLuint program)
{
	BROFILER_CATEGORY("Render Lights", Profiler::Color::White);

	RenderDirectionalLight();
	RenderSpotLights(program);
	RenderPointLights(program);
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

				++current_number_directional_lights_rendered;
			}
		}
	}

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	size_t num_directional_lights_offset = App->program->uniform_buffer.lights_uniform_offset + 7 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, num_directional_lights_offset, sizeof(float), &current_number_directional_lights_rendered);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ModuleLight::RenderSpotLights(GLuint program)
{
	current_number_spot_lights_rendered = 0;
	for (auto& light : lights)
	{
		if (light->IsEnabled())
		{
			if (light->light_type == ComponentLight::LightType::SPOT_LIGHT)
			{
				float3 light_color_scaled = light->light_intensity * float3(light->light_color);
				std::string spot_light_current_uniform_name = "spot_lights[" + std::to_string(current_number_spot_lights_rendered) + "]";

				glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".color").c_str()), 1, light_color_scaled.ptr());
				glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".position").c_str()), 1, light->owner->transform.GetGlobalTranslation().ptr());
				glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".direction").c_str()), 1, light->owner->transform.GetFrontVector().ptr());
				float cutOff_value = cos((12.5f * 3, 1415) / 180);
				glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".cutOff").c_str()), cutOff_value);
				float outerCutOff_value = cos((17.5f * 3, 1415) / 180);
				glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".outerCutOff").c_str()), outerCutOff_value);
				float constant_value = 1.0F;
				glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".constant").c_str()), constant_value);
				float linear_value = 0.027F;
				glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".linear").c_str()), linear_value);
				float quadratic_value = 0.0028F;
				glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".quadratic").c_str()), quadratic_value);

				++current_number_spot_lights_rendered;
			}
		}
	}

	glUniform1i(glGetUniformLocation(program, "num_spot_lights"), current_number_spot_lights_rendered);
}

void ModuleLight::RenderPointLights(GLuint program)
{
	current_number_point_lights_rendered = 0;
	for (auto& light : lights)
	{
		if (light->IsEnabled())
		{
			if (light->light_type == ComponentLight::LightType::POINT_LIGHT)
			{
				float3 light_color_scaled = light->light_intensity * float3(light->light_color);
				std::string point_light_current_uniform_name = "point_lights[" + std::to_string(current_number_point_lights_rendered) + "]";

				glUniform3fv(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".color").c_str()), 1, light_color_scaled.ptr());
				glUniform3fv(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".position").c_str()), 1, light->owner->transform.GetGlobalTranslation().ptr());
				float constant_value = 1.0F;
				glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".constant").c_str()), constant_value);
				float linear_value = 0.027F;
				glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".linear").c_str()), linear_value);
				float quadratic_value = 0.0028F;
				glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".quadratic").c_str()), quadratic_value);

				++current_number_point_lights_rendered;
			}
		}
	}

	glUniform1i(glGetUniformLocation(program, "num_point_lights"), current_number_point_lights_rendered);
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