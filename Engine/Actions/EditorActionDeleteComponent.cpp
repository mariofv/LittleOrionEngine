#include "EditorActionDeleteComponent.h"
#include "GameObject.h"


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
}

void EditorActionDeleteComponent::Redo()
{
	component->Disable();
	auto it = std::find(component->owner->components.begin(), component->owner->components.end(), component);
	component->owner->components.erase(it);
}
