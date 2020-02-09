#include "EditorActionAddComponent.h"
#include "GameObject.h"


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
	
}

void EditorActionAddComponent::Redo()
{
	component->Enable();
	component->owner->components.push_back(component);
}
