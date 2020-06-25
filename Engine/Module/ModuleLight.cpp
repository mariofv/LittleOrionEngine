#include "ModuleLight.h"
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
	//directional_light_rotation = Quat::identity;


	return true;
}





update_status ModuleLight::PostUpdate()
{

	light_position = float3((light_aabb.maxPoint.x + light_aabb.minPoint.x) * 0.5, (light_aabb.maxPoint.y + light_aabb.minPoint.y) * 0.5, light_aabb.maxPoint.z);
	
	float3 new_pos;
	new_pos = directional_light_rotation * light_position;


	App->cameras->dir_light_game_object->transform.SetRotation(directional_light_rotation);
	App->cameras->dir_light_game_object->transform.SetTranslation(new_pos);

	App->cameras->UpdateDirectionalLightFrustums(light_aabb.maxPoint, light_aabb.minPoint);

	light_aabb.SetNegativeInfinity();
	light_obb.SetNegativeInfinity();
	
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
	BROFILER_CATEGORY("Render Lights", Profiler::Color::White);

	RenderDirectionalLight(mesh_position);
	RenderSpotLights(mesh_position, program);
	RenderPointLights(mesh_position, program);
	SendShadowMatricesToShader(program);

}

void ModuleLight::RenderDirectionalLight(const float3& mesh_position)
{
	SortClosestLights(mesh_position, ComponentLight::LightType::DIRECTIONAL_LIGHT);
	current_number_directional_lights_rendered = 0;
	unsigned int i = 0;
	while (current_number_directional_lights_rendered < MAX_DIRECTIONAL_LIGHTS_RENDERED && i < closest_lights.size())
	{
		ComponentLight* light = closest_lights[i].second;

		float3 light_color_scaled = light->light_intensity * float3(light->light_color);

		glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

		glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.lights_uniform_offset, sizeof(float3), light_color_scaled.ptr());

		size_t light_direction_offset = App->program->uniform_buffer.lights_uniform_offset + 4 * sizeof(float);
		glBufferSubData(GL_UNIFORM_BUFFER, light_direction_offset, sizeof(float3), light->owner->transform.GetFrontVector().ptr());
	
		directional_light_rotation = directional_light_rotation.LookAt(float3::unitZ, light->owner->transform.GetFrontVector(), float3::unitY, light->owner->transform.GetUpVector());
		

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		++current_number_directional_lights_rendered;
		++i;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);
	size_t num_directional_lights_offset = App->program->uniform_buffer.lights_uniform_offset + 7 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, num_directional_lights_offset, sizeof(int), &current_number_directional_lights_rendered);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ModuleLight::RenderSpotLights(const float3& mesh_position, GLuint program)
{
	SortClosestLights(mesh_position, ComponentLight::LightType::SPOT_LIGHT);
	current_number_spot_lights_rendered = 0;
	unsigned int i = 0;
	while (current_number_spot_lights_rendered < MAX_SPOT_LIGHTS_RENDERED && i < closest_lights.size())
	{
		ComponentLight* light = closest_lights[i].second;

		float3 light_color_scaled = light->light_intensity * float3(light->light_color);
		std::string spot_light_current_uniform_name = "spot_lights[" + std::to_string(current_number_spot_lights_rendered) + "]";

		glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".color").c_str()), 1, light_color_scaled.ptr());
		glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".position").c_str()), 1, light->owner->transform.GetGlobalTranslation().ptr());
		glUniform3fv(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".direction").c_str()), 1, light->owner->transform.GetFrontVector().ptr());
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".cutOff").c_str()), light->spot_light_parameters.cutoff);
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".outerCutOff").c_str()), light->spot_light_parameters.outer_cutoff);
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".constant").c_str()), light->spot_light_parameters.constant);
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".linear").c_str()), light->spot_light_parameters.linear);
		glUniform1f(glGetUniformLocation(program, std::string(spot_light_current_uniform_name + ".quadratic").c_str()), light->spot_light_parameters.quadratic);

		++current_number_spot_lights_rendered;
		++i;
	}

	glUniform1i(glGetUniformLocation(program, "num_spot_lights"), current_number_spot_lights_rendered);
}

void ModuleLight::SendShadowMatricesToShader(GLuint program)
{
	glUniformMatrix4fv(glGetUniformLocation(program, "close_directional_view"), 1, GL_TRUE, &App->cameras->directional_light_camera->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "close_directional_proj"), 1, GL_TRUE, &App->cameras->directional_light_camera->GetProjectionMatrix()[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "mid_directional_view"), 1, GL_TRUE, &App->cameras->directional_light_mid->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "mid_directional_proj"), 1, GL_TRUE, &App->cameras->directional_light_mid->GetProjectionMatrix()[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(program, "far_directional_view"), 1, GL_TRUE, &App->cameras->directional_light_far->GetViewMatrix()[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "far_directional_proj"), 1, GL_TRUE, &App->cameras->directional_light_far->GetProjectionMatrix()[0][0]);

	if (App->cameras->main_camera != nullptr)
	{
		//glUniformMatrix4fv(glGetUniformLocation(program, "main_cam_view"), 1, GL_TRUE, &App->cameras->main_camera->GetViewMatrix()[0][0]);
		//glUniformMatrix4fv(glGetUniformLocation(program, "main_cam_proj"), 1, GL_TRUE, &App->cameras->main_camera->GetProjectionMatrix()[0][0]);

		/*glUniformMatrix4fv(glGetUniformLocation(program, "close_cam_view"), 1, GL_TRUE, &App->cameras->camera_close->GetViewMatrix()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "close_cam_proj"), 1, GL_TRUE, &App->cameras->camera_close->GetProjectionMatrix()[0][0]);

		glUniformMatrix4fv(glGetUniformLocation(program, "mid_cam_view"), 1, GL_TRUE, &App->cameras->camera_mid->GetViewMatrix()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "mid_cam_proj"), 1, GL_TRUE, &App->cameras->camera_mid->GetProjectionMatrix()[0][0]);*/

		glUniform1f(glGetUniformLocation(program, "far_plane"), App->cameras->main_camera->camera_frustum.farPlaneDistance);

	}

}

void ModuleLight::UpdateLightAABB(GameObject& object)
{
	//Light aabb will enclose every object in the scene
	AABB temp;
	temp = object.aabb.bounding_box;

	object_obb = object.aabb.bounding_box.Transform(directional_light_rotation.Inverted());

	AABB object_aabb = object_obb.MinimalEnclosingAABB();

	light_aabb.Enclose(object_aabb);

	light_obb = light_aabb.Transform(directional_light_rotation);
}

void ModuleLight::RenderPointLights(const float3& mesh_position, GLuint program)
{
	SortClosestLights(mesh_position, ComponentLight::LightType::POINT_LIGHT);
	current_number_point_lights_rendered = 0;
	unsigned int i = 0;
	while (current_number_point_lights_rendered < MAX_POINT_LIGHTS_RENDERED && i < closest_lights.size())
	{
		ComponentLight* light = closest_lights[i].second;

		float3 light_color_scaled = light->light_intensity * float3(light->light_color);
		std::string point_light_current_uniform_name = "point_lights[" + std::to_string(current_number_point_lights_rendered) + "]";

		glUniform3fv(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".color").c_str()), 1, light_color_scaled.ptr());
		glUniform3fv(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".position").c_str()), 1, light->owner->transform.GetGlobalTranslation().ptr());
		glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".constant").c_str()), light->point_light_parameters.constant);
		glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".linear").c_str()), light->point_light_parameters.linear);
		glUniform1f(glGetUniformLocation(program, std::string(point_light_current_uniform_name + ".quadratic").c_str()), light->point_light_parameters.quadratic);

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
	auto& it = std::find(lights.begin(), lights.end(), light_to_remove);
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
		return lhs.first < rhs.first;
	});
}