#include "ModuleLight.h"
#include "Application.h"
#include "Component/ComponentLight.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleModelLoader.h"

ModuleLight::~ModuleLight()
{
	CleanUp();
}


bool ModuleLight::Init()
{
	GameObject * light_gameobject = App->model_loader->LoadCoreModel(PRIMITIVE_SPHERE_PATH);
	light_gameobject->name = "Light";
	ComponentMaterial * material = (ComponentMaterial*)light_gameobject->CreateComponent(Component::ComponentType::LIGHT);
	light_gameobject->transform.SetTranslation(float3(4.f,1.5f,-1.5f));
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