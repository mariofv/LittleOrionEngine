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

bool ModuleLight::CleanUp()
{
	for (auto& light : lights)
	{
		light->owner->RemoveComponent(light);
	}
	lights.clear();
	return true;
}

void ModuleLight::RenderLight()
{
	current_lights_rendered = 0;

	BROFILER_CATEGORY("Render Lights", Profiler::Color::White);
	for (auto& light : lights)
	{
		if (light->IsEnabled())
		{
			light->Render();
			++current_lights_rendered;
		}
	}

	if (current_lights_rendered == 0)
	{
		RenderDarkness(); // TODO: This needs to be changed with ambiental light
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
	auto it = std::find(lights.begin(), lights.end(), light_to_remove);
	if (it != lights.end())
	{
		delete *it;
		lights.erase(it);
	}
}

void ModuleLight::RenderDarkness() const
{
	float3 darkness_color = float3(0.f);
	float3 darkness_position = float3(0.f);

	glBindBuffer(GL_UNIFORM_BUFFER, App->program->uniform_buffer.ubo);

	glBufferSubData(GL_UNIFORM_BUFFER, App->program->uniform_buffer.lights_uniform_offset, 3 * sizeof(float), darkness_color.ptr());

	size_t light_position_offset = App->program->uniform_buffer.lights_uniform_offset + 4 * sizeof(float);
	glBufferSubData(GL_UNIFORM_BUFFER, light_position_offset, 3 * sizeof(float), darkness_position.ptr());

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}