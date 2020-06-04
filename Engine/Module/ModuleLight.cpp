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
	directional_light_rotation = Quat::identity;

	light_aabb_game_object = App->scene->CreateGameObject();
	light_aabb_game_object->transform.SetTranslation(float3(0, 0, 0));

	light_transform = App->scene->CreateGameObject();
	light_transform->transform.SetTranslation(float3(0, 0, 0));

	light_aabb = new ComponentAABB(light_aabb_game_object);
	light_aabb->bounding_box.SetNegativeInfinity();
	light_aabb->GenerateBoundingBox();

	return true;
}

update_status ModuleLight::PostUpdate()
{
	light_aabb->bounding_box.SetNegativeInfinity();
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
	
		directional_light_rotation = directional_light_rotation.LookAt(float3::unitZ, light->owner->transform.GetFrontVector(), light->owner->transform.GetUpVector(), float3::unitY);
		light_transform->transform.SetRotation(directional_light_rotation);
		//App->cameras->light_aabb_game_object->transform.SetRotation(directional_light_rotation); //Directional light rotation
		//light->owner->transform.SetTranslation(App->cameras->dir_light_game_object->transform.GetTranslation());

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
		glUniformMatrix4fv(glGetUniformLocation(program, "main_cam_view"), 1, GL_TRUE, &App->cameras->main_camera->GetViewMatrix()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "main_cam_proj"), 1, GL_TRUE, &App->cameras->main_camera->GetProjectionMatrix()[0][0]);

		glUniformMatrix4fv(glGetUniformLocation(program, "close_cam_view"), 1, GL_TRUE, &App->cameras->camera_close->GetViewMatrix()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "close_cam_proj"), 1, GL_TRUE, &App->cameras->camera_close->GetProjectionMatrix()[0][0]);

		glUniformMatrix4fv(glGetUniformLocation(program, "mid_cam_view"), 1, GL_TRUE, &App->cameras->camera_mid->GetViewMatrix()[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "mid_cam_proj"), 1, GL_TRUE, &App->cameras->camera_mid->GetProjectionMatrix()[0][0]);

		glUniform1f(glGetUniformLocation(program, "main_cam_far_plane"), App->cameras->camera_close->camera_frustum.farPlaneDistance);

	}

}

void ModuleLight::UpdateLightAABB(GameObject& object)
{


	//Light aabb will enclose every object in the scene
	AABB temp;
	temp = object.aabb.bounding_box;
	
	OBB object_obb;
	object_obb.SetFrom(temp); //get translation and rotation from the GO
	float3 obb_last_pos = object_obb.CenterPoint();

	object_obb.Translate(-object_obb.CenterPoint()); //translate to the origin
	object_obb.Transform(directional_light_rotation);		  //rotate
	object_obb.Translate(obb_last_pos);  //translate to its original position


	light_aabb->bounding_box.Enclose(object_obb);

	

	light_obb.SetFrom(light_aabb->bounding_box);
	light_obb.Translate(-light_aabb->bounding_box.CenterPoint()); //translate to the origin
	light_obb.Transform(directional_light_rotation);		  //rotate
	light_obb.Scale(light_obb.CenterPoint(), float3(1.5, 1.5, 1.5));
	light_obb.Translate(light_aabb->bounding_box.CenterPoint()); //translate to the origin

	

	
	//Create max and min points to create camera frustum

	float MaxX = light_obb.CornerPoint(3).x;
	float MaxY = light_obb.CornerPoint(3).y;
	float MaxZ = light_obb.CornerPoint(3).z;

	float minX = light_obb.CornerPoint(4).x;
	float minY = light_obb.CornerPoint(4).y;
	float minZ = light_obb.CornerPoint(4).z;

	obb_max_point.x = MaxX;
	obb_max_point.y = MaxY;
	obb_max_point.z = MaxZ;

	obb_min_point.x = minX;
	obb_min_point.y = minY;
	obb_min_point.z = minZ;

	//Light position at the far plane of the aabb, in the middle
	light_position = float3((obb_max_point.x + obb_min_point.x) * 0.5, (obb_max_point.y + obb_min_point.y) * 0.5, obb_max_point.z);
	light_transform->transform.SetTranslation(light_position);
	
	//App->cameras->dir_light_game_object->transform.SetRotation(directional_light_rotation);
	App->cameras->dir_light_game_object->transform.SetTranslation(light_position);

	App->cameras->UpdateDirectionalLightFrustums(obb_max_point, obb_min_point);
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