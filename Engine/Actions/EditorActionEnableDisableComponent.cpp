#include "EditorActionEnableDisableComponent.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionEnableDisableComponent::EditorActionEnableDisableComponent(Component* comp)
{
	UUID_COMP = comp->UUID;
}

void EditorActionEnableDisableComponent::Undo()
{
	Component* component = App->scene->GetComponent(UUID_COMP);

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
