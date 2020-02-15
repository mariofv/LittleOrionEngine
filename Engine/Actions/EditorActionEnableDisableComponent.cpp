#include "EditorActionEnableDisableComponent.h"



EditorActionEnableDisableComponent::EditorActionEnableDisableComponent(Component* comp)
{
	component = comp;
}


EditorActionEnableDisableComponent::~EditorActionEnableDisableComponent()
{
}

void EditorActionEnableDisableComponent::Undo()
{
	if(component->IsEnabled())
	{
		component->Disable();
	}
	else
	{
		component->Enable();
	}
}

void EditorActionEnableDisableComponent::Redo()
{
	Undo();
}
