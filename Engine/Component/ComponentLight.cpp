#include "ComponentLight.h"
#include "Application.h"
#include "Module/ModuleTexture.h"
#include "UI/ComponentsUI.h"


ComponentLight::ComponentLight() : Component(nullptr, ComponentType::LIGHT)
{
}

ComponentLight::ComponentLight(GameObject * owner) : Component(owner, ComponentType::LIGHT)
{

}

void ComponentLight::Delete()
{
	App->texture->RemoveComponentLight(this);
}

void ComponentLight::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddInt((unsigned int)type, "ComponentType");
	config.AddBool(active, "Active");
	
}

void ComponentLight::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

}

void ComponentLight::ShowComponentWindow()
{
	ComponentsUI::ShowComponentLightWindow(this);
}