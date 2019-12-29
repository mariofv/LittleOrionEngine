#include "ModuleLight.h"
#include "Application.h"
#include "Component/ComponentLight.h"
#include "Module/ModuleScene.h"

ModuleLight::~ModuleLight()
{
	CleanUp();
}


bool ModuleLight::Init()
{
	GameObject * light_gameobject = App->scene->CreateChildGameObject(App->scene->GetRoot());
	light_gameobject->name = "Light";
	light_gameobject->CreateComponent(Component::ComponentType::LIGHT);
	return true;
}
bool ModuleLight::CleanUp()
{
	for (auto& light : lights)
	{
		delete light;
	}
	lights.clear();
	return true;
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