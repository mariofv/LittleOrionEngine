#include "EditorActionAddComponent.h"
#include "Main/GameObject.h"
#include "Module/ModuleLight.h"
#include "Main/Application.h"

EditorActionAddComponent::EditorActionAddComponent(Component* comp)
{
	component = comp;
}


EditorActionAddComponent::~EditorActionAddComponent()
{
	if (component != nullptr && !component->IsEnabled())
		component->Delete();
}

void EditorActionAddComponent::Undo()
{
	component->Disable();
	auto it = std::find(component->owner->components.begin(), component->owner->components.end(), component);
	component->owner->components.erase(it);

	if(component->type == Component::ComponentType::LIGHT)
	{
		auto it = std::find(App->lights->lights.begin(), App->lights->lights.end(), (ComponentLight*)component);
		if (it != App->lights->lights.end())
		{
			App->lights->lights.erase(it);
		}
	}
}

void EditorActionAddComponent::Redo()
{
	component->Enable();
	component->owner->components.push_back(component);

	if (component->type == Component::ComponentType::LIGHT)
	{
		App->lights->lights.push_back((ComponentLight*)component);
	}

}
