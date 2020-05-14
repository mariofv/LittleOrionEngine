#include "ComponentEventSystem.h"

#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleWindow.h"

#include <queue>


ComponentEventSystem::ComponentEventSystem() : Component(ComponentType::CANVAS)
{
	
}

ComponentEventSystem::ComponentEventSystem(GameObject* owner) : Component(owner, ComponentType::CANVAS)
{
	
}

ComponentEventSystem& ComponentEventSystem::operator=(const ComponentEventSystem& component_to_copy)
{
	Component::operator = (component_to_copy);
	return *this;
}

Component* ComponentEventSystem::Clone(bool original_prefab) const
{
	ComponentEventSystem* created_component;
	if (original_prefab)
	{
		created_component = new ComponentEventSystem();
	}
	else
	{
		created_component = App->ui->CreateComponentEventSystem();
	}
	*created_component = *this;
	return created_component;
};

void ComponentEventSystem::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentEventSystem*>(component_to_copy) = *this;
}

void ComponentEventSystem::Delete()
{
	App->ui->RemoveComponentEventSystem(this);
}


void ComponentEventSystem::SpecializedSave(Config& config) const
{
}

void ComponentEventSystem::SpecializedLoad(const Config& config)
{
}