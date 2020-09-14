#include "ModuleLight.h"

#include "Log/EngineLog.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"

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

update_status ModuleLight::PostUpdate()
{
	BROFILER_CATEGORY("Module Light PostUpdate", Profiler::Color::PaleTurquoise);
	light_position = float3((light_aabb.maxPoint.x + light_aabb.minPoint.x) * 0.5, (light_aabb.maxPoint.y + light_aabb.minPoint.y) * 0.5, light_aabb.maxPoint.z);
	
	float3 new_pos;
	new_pos = directional_light_rotation * light_position;

	light_aabb.SetNegativeInfinity();
	
	return update_status::UPDATE_CONTINUE;

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
	BROFILER_CATEGORY("Module Light Render", Profiler::Color::White);
	SortClosestLights(mesh_position);
	SendShadowUniformsToShader(program);

	current_number_directional_lights_rendered = 0;
	current_number_spot_lights_rendered = 0;
	current_number_point_lights_rendered = 0;
	for (const ComponentLight* light : lights)
	{
		const ComponentLight& light_ref = *light;
		RenderDirectionalLight(light_ref);
		RenderSpotLights(light_ref, program);
		RenderPointLights(light_ref, program);
	}

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	size_t num_directional_lights_offset = App->program->uniform_buffer.lights_uniform_offset + 7 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, num_directional_lights_offset, sizeof(int), &current_number_directional_lights_rendered);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUniform1i(glGetUniformLocation(program, "num_spot_lights"), current_number_spot_lights_rendered);
	glUniform1i(glGetUniformLocation(program, "num_point_lights"), current_number_point_lights_rendered);
}

void ModuleLight::RenderDirectionalLight(const ComponentLight& light)
{
	BROFILER_CATEGORY("Module Light Render Directional lights", Profiler::Color::YellowGreen);
	if (light.light_type == ComponentLight::LightType::DIRECTIONAL_LIGHT && current_number_directional_lights_rendered < MAX_DIRECTIONAL_LIGHTS_RENDERED && light.active)
	{

		float3 light_color_scaled = light.light_intensity * float3(light.light_color);

		glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

		glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.lights_uniform_offset, sizeof(float3), light_color_scaled.ptr());

		size_t light_direction_offset = App->program->uniform_buffer.lights_uniform_offset + 4 * sizeof(float);
		glBufferSubData(GL_UNIFORM_BUFFER, light_direction_offset, sizeof(float3), light.owner->transform.GetFrontVector().ptr());

		directional_light_rotation = directional_light_rotation.LookAt(float3::unitZ, light.owner->transform.GetFrontVector(), float3::unitY, light.owner->transform.GetUpVector());


		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		++current_number_directional_lights_rendered;
	}
}

void ModuleLight::RenderSpotLights(const ComponentLight& light, GLuint program)
{
	BROFILER_CATEGORY("Module Light Render spot lights", Profiler::Color::PaleTurquoise);
	if (light.light_type == ComponentLight::LightType::SPOT_LIGHT && current_number_spot_lights_rendered < MAX_SPOT_LIGHTS_RENDERED && light.active)
	{
		float3 light_color_scaled = light.light_intensity * float3(light.light_color);
		std::string spot_light_current_uniform_name = "spot_lights[" + std::to_string(current_number_spot_lights_rendered) + "]";

		glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".color").c_str()), 1, light_color_scaled.ptr());
		glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".position").c_str()), 1, light.owner->transform.GetGlobalTranslation().ptr());
		glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".direction").c_str()), 1, light.owner->transform.GetFrontVector().ptr());
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".cutOff").c_str()), light.spot_light_parameters.cutoff);
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".outerCutOff").c_str()), light.spot_light_parameters.outer_cutoff);
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".constant").c_str()), light.spot_light_parameters.constant);
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".linear").c_str()), light.spot_light_parameters.linear);
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".quadratic").c_str()), light.spot_light_parameters.quadratic);

		++current_number_spot_lights_rendered;
	}
}



void ModuleLight::RenderPointLights(const ComponentLight& light, GLuint program)
{
	BROFILER_CATEGORY("Module Light Render point lights", Profiler::Color::AliceBlue);
	if (light.light_type == ComponentLight::LightType::POINT_LIGHT && current_number_point_lights_rendered < MAX_POINT_LIGHTS_RENDERED && light.active)
	{
		float3 light_color_scaled = light.light_intensity * float3(light.light_color);
		std::string point_light_current_uniform_name = "point_lights[" + std::to_string(current_number_point_lights_rendered) + "]";

		glUniform3fv(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".color").c_str()), 1, light_color_scaled.ptr());
		glUniform3fv(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".position").c_str()), 1, light.owner->transform.GetGlobalTranslation().ptr());
		glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".constant").c_str()), light.point_light_parameters.constant);
		glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".linear").c_str()), light.point_light_parameters.linear);
		glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".quadratic").c_str()), light.point_light_parameters.quadratic);

		++current_number_point_lights_rendered;
	}

}

ComponentLight* ModuleLight::CreateComponentLight()
{
	ComponentLight * created_light = new ComponentLight();
	lights.push_back(created_light);
	return created_light;
}

void ModuleLight::RemoveComponentLight(ComponentLight* light_to_remove)
{
	auto& it = std::find(lights.begin(), lights.end(), light_to_remove);
	if (it != lights.end())
	{
		delete *it;
		lights.erase(it);
	}
}

void ModuleLight::SortClosestLights(const float3& position)
{
	BROFILER_CATEGORY("Module Light Sort ligths", Profiler::Color::Magenta);
	std::sort(lights.begin(), lights.end(), [position](const  ComponentLight* lhs, const ComponentLight* rhs)
	{
		float distance1 = position.Distance(lhs->owner->transform.GetGlobalTranslation());
		float distance2 = position.Distance(rhs->owner->transform.GetGlobalTranslation());
		return lhs->IsEnabled() && distance1 < distance2;
	});
}


void ModuleLight::SendShadowUniformsToShader(GLuint program)
{
	BROFILER_CATEGORY("Module Light shadow uniforms", Profiler::Color::MediumTurquoise);
	glUniform1i(glGetUniformLocation(program, "render_shadows"), false);

	/*
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, directional_light_camera->depth_map);
	glUniform1i(glGetUniformLocation(program, "close_depth_map"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, directional_light_mid->depth_map);
	glUniform1i(glGetUniformLocation(program, "mid_depth_map"), 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, directional_light_far->depth_map);
	glUniform1i(glGetUniformLocation(program, "far_depth_map"), 2);


	glUniformMatrix4fv(glGetUniformLocation(program, "close_directional_view"), 1, GL_TRUE, &directional_light_camera->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "close_directional_proj"), 1, GL_TRUE, &directional_light_camera->GetProjectionMatrix()[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "mid_directional_view"), 1, GL_TRUE, &directional_light_mid->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "mid_directional_proj"), 1, GL_TRUE, &directional_light_mid->GetProjectionMatrix()[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "far_directional_view"), 1, GL_TRUE, &directional_light_far->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "far_directional_proj"), 1, GL_TRUE, &directional_light_far->GetProjectionMatrix()[0][0]);
	*/

	if (App->cameras->main_camera != nullptr)
	{
		glUniform1f(glGetUniformLocation(program, "far_plane"), App->cameras->main_camera->camera_frustum.farPlaneDistance);

	}

}

