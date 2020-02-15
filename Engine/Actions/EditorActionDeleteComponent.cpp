#include "EditorActionDeleteComponent.h"
#include "Main/GameObject.h"
#include "Module/ModuleLight.h"
#include "Main/Application.h"

EditorActionDeleteComponent::EditorActionDeleteComponent(Component* comp)
{
	component = comp;
}


EditorActionDeleteComponent::~EditorActionDeleteComponent()
{
	if(component != nullptr && !component->IsEnabled())
		component->Delete();
}

void EditorActionDeleteComponent::Undo()
{
	component->Enable();
	component->owner->components.push_back(component);

	if (component->type == Component::ComponentType::LIGHT)
	{
		App->lights->lights.push_back((ComponentLight*)component);
	}
}

void EditorActionDeleteComponent::Redo()
{
	component->Disable();
	auto it = std::find(component->owner->components.begin(), component->owner->components.end(), component);
	component->owner->components.erase(it);


	if (component->type == Component::ComponentType::LIGHT)
	{
		auto it = std::find(App->lights->lights.begin(), App->lights->lights.end(), (ComponentLight*)component);
		if (it != App->lights->lights.end())
		{
			App->lights->lights.erase(it);
		}
	}
}
