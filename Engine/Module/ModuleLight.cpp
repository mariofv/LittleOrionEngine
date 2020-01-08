#include "ModuleLight.h"
#include "Application.h"
#include "Component/ComponentLight.h"
#include "GameObject.h"

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
void ModuleLight::RenderLight() const
{
	if (lights.size() > 0)
	{
		lights[0]->Render();
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