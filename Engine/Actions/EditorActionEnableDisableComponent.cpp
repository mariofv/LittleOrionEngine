#include "EditorActionEnableDisableComponent.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionEnableDisableComponent::EditorActionEnableDisableComponent(Component* comp) : component_UUID(comp->UUID){}

void EditorActionEnableDisableComponent::Undo()
{
	Component* component = App->scene->GetComponent(component_UUID);

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
