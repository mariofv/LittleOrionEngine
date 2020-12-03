#include "ModuleLight.h"

#include "Component/ComponentLight.h"
#include "Log/EngineLog.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Rendering/LightFrustum.h"

#include <Brofiler/Brofiler.h>
#include <algorithm>

ModuleLight::~ModuleLight()
{
	CleanUp();
}

bool ModuleLight::Init()
{
	APP_LOG_SECTION("************ Module Light Init ************");

	full_frustum = new LightFrustum(LightFrustum::FrustumSubDivision::FULL_FRUSTUM);
	near_frustum = new LightFrustum(LightFrustum::FrustumSubDivision::NEAR_FRUSTUM);
	mid_frustum = new LightFrustum(LightFrustum::FrustumSubDivision::MID_FRUSTUM);
	far_frustum = new LightFrustum(LightFrustum::FrustumSubDivision::FAR_FRUSTUM);

	return true;
}

update_status ModuleLight::Update()
{
	BROFILER_CATEGORY("Module Light PostUpdate", Profiler::Color::PaleTurquoise);
	if (!App->renderer->shadows_enabled)
	{
		return update_status::UPDATE_CONTINUE;
	}

	full_frustum->Update();
	near_frustum->Update();
	mid_frustum->Update();
	far_frustum->Update();

	return update_status::UPDATE_CONTINUE;

}

bool ModuleLight::CleanUp()
{
	for (auto& light : lights)
	{
		light->owner->RemoveComponent(light);
	}
	lights.clear();

	delete full_frustum;
	delete near_frustum;
	delete mid_frustum;
	delete far_frustum;

	return true;
}

void ModuleLight::Render(const float3& mesh_position, GLuint program)
{
	BROFILER_CATEGORY("Module Light Render", Profiler::Color::White);
	SortClosestLights(mesh_position);

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

void ModuleLight::BindLightFrustumsMatrices()
{
	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

	static size_t full_depth_space_matrix_offset = App->program->uniform_buffer.light_frustums_uniform_offset;
	float4x4 full_depth_space_matrix = full_frustum->light_orthogonal_frustum.ProjectionMatrix() * full_frustum->light_orthogonal_frustum.ViewMatrix();
	glBufferSubData(GL_UNIFORM_BUFFER, full_depth_space_matrix_offset, sizeof(float4x4), full_depth_space_matrix.Transposed().ptr());

	static size_t near_depth_space_matrix_offset = App->program->uniform_buffer.light_frustums_uniform_offset + sizeof(float4x4);
	float4x4 near_depth_space_matrix = near_frustum->light_orthogonal_frustum.ProjectionMatrix() * near_frustum->light_orthogonal_frustum.ViewMatrix();
	glBufferSubData(GL_UNIFORM_BUFFER, near_depth_space_matrix_offset, sizeof(float4x4), near_depth_space_matrix.Transposed().ptr());

	static size_t mid_depth_space_matrix_offset = App->program->uniform_buffer.light_frustums_uniform_offset + 2 * sizeof(float4x4);
	float4x4 mid_depth_space_matrix = mid_frustum->light_orthogonal_frustum.ProjectionMatrix() * mid_frustum->light_orthogonal_frustum.ViewMatrix();
	glBufferSubData(GL_UNIFORM_BUFFER, mid_depth_space_matrix_offset, sizeof(float4x4), mid_depth_space_matrix.Transposed().ptr());

	static size_t far_depth_space_matrix_offset = App->program->uniform_buffer.light_frustums_uniform_offset + 3 * sizeof(float4x4);
	float4x4 far_depth_space_matrix = far_frustum->light_orthogonal_frustum.ProjectionMatrix() * far_frustum->light_orthogonal_frustum.ViewMatrix();
	glBufferSubData(GL_UNIFORM_BUFFER, far_depth_space_matrix_offset, sizeof(float4x4), far_depth_space_matrix.Transposed().ptr());

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
