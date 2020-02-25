#include "ModuleLight.h"
#include "Main/Application.h"
#include "Module/ModuleProgram.h"
#include "Main/GameObject.h"

#include <Brofiler/Brofiler.h>
#include <algorithm>

ModuleLight::~ModuleLight()
{
	CleanUp();
}

bool ModuleLight::Init()
{
	APP_LOG_SECTION("************ Module Light Init ************");

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

void ModuleLight::Render(const float3& mesh_position, GLuint program)
{
	BROFILER_CATEGORY("Render Lights", Profiler::Color::White);

	RenderDirectionalLight(mesh_position);
	RenderSpotLights(mesh_position, program);
	RenderPointLights(mesh_position, program);
}

void ModuleLight::RenderDirectionalLight(const float3& mesh_position)
{
	SortClosestLights(mesh_position, ComponentLight::LightType::DIRECTIONAL_LIGHT);
	current_number_directional_lights_rendered = 0;
	int i = 0;
	while (current_number_directional_lights_rendered < MAX_DIRECTIONAL_LIGHTS_RENDERED && i < closest_lights.size())
	{
		ComponentLight* light = closest_lights[i].second;

		float3 light_color_scaled = light->light_intensity * float3(light->light_color);

		glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

		glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.lights_uniform_offset, sizeof(float3), light_color_scaled.ptr());

		size_t light_direction_offset = App->program->uniform_buffer.lights_uniform_offset + 4 * sizeof(float);
		glBufferSubData(GL_UNIFORM_BUFFER, light_direction_offset, sizeof(float3), light->owner->transform.GetFrontVector().ptr());

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		++current_number_directional_lights_rendered;
		++i;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	size_t num_directional_lights_offset = App->program->uniform_buffer.lights_uniform_offset + 7 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, num_directional_lights_offset, sizeof(float), &current_number_directional_lights_rendered);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ModuleLight::RenderSpotLights(const float3& mesh_position, GLuint program)
{
	SortClosestLights(mesh_position, ComponentLight::LightType::SPOT_LIGHT);
	current_number_spot_lights_rendered = 0;
	int i = 0;
	while (current_number_spot_lights_rendered < MAX_SPOT_LIGHTS_RENDERED && i < closest_lights.size())
	{
		ComponentLight* light = closest_lights[i].second;

		float3 light_color_scaled = light->light_intensity * float3(light->light_color);
		std::string spot_light_current_uniform_name = "spot_lights[" + std::to_string(current_number_spot_lights_rendered) + "]";

		glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".color").c_str()), 1, light_color_scaled.ptr());
		glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".position").c_str()), 1, light->owner->transform.GetGlobalTranslation().ptr());
		glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".direction").c_str()), 1, light->owner->transform.GetFrontVector().ptr());
		float cutOff_value = light->spot_light_parameters.cutoff;
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".cutOff").c_str()), cutOff_value);
		float outerCutOff_value = light->spot_light_parameters.outer_cutoff;
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".outerCutOff").c_str()), outerCutOff_value);
		float constant_value = light->spot_light_parameters.constant;
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".constant").c_str()), constant_value);
		float linear_value = light->spot_light_parameters.linear;
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".linear").c_str()), linear_value);
		float quadratic_value = light->spot_light_parameters.quadratic;
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".quadratic").c_str()), quadratic_value);

		++current_number_spot_lights_rendered;
		++i;
	}

	glUniform1i(glGetUniformLocation(program, "num_spot_lights"), current_number_spot_lights_rendered);
}

void ModuleLight::RenderPointLights(const float3& mesh_position, GLuint program)
{
	SortClosestLights(mesh_position, ComponentLight::LightType::POINT_LIGHT);
	current_number_point_lights_rendered = 0;
	int i = 0;
	while (current_number_point_lights_rendered < MAX_POINT_LIGHTS_RENDERED && i < closest_lights.size())
	{
		ComponentLight* light = closest_lights[i].second;

		float3 light_color_scaled = light->light_intensity * float3(light->light_color);
		std::string point_light_current_uniform_name = "point_lights[" + std::to_string(current_number_point_lights_rendered) + "]";

		glUniform3fv(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".color").c_str()), 1, light_color_scaled.ptr());
		glUniform3fv(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".position").c_str()), 1, light->owner->transform.GetGlobalTranslation().ptr());
		float constant_value = light->point_light_parameters.constant;
		glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".constant").c_str()), constant_value);
		float linear_value = light->point_light_parameters.linear;
		glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".linear").c_str()), linear_value);
		float quadratic_value = light->point_light_parameters.quadratic;
		glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".quadratic").c_str()), quadratic_value);

		++current_number_point_lights_rendered;
		++i;
	}

	glUniform1i(glGetUniformLocation(program, "num_point_lights"), current_number_point_lights_rendered);
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

void ModuleLight::SortClosestLights(const float3& position, ComponentLight::LightType light_type)
{
	closest_lights = std::vector< std::pair<float, ComponentLight*> >(0);
	for (auto& light : lights)
	{
		if (light->IsEnabled() && light->light_type == light_type)
		{
			float distance = position.Distance(light->owner->transform.GetGlobalTranslation());
			closest_lights.push_back(std::pair<float, ComponentLight*>(distance, light));
		}
	}

	std::sort(closest_lights.begin(), closest_lights.end(), [](const std::pair<float, ComponentLight*>& lhs, const std::pair<float, ComponentLight*>& rhs)
	{
		return lhs.second < rhs.second;
	});
}