#include "EditorActionDeleteComponent.h"
#include "Main/GameObject.h"
#include "Module/ModuleLight.h"
#include "Main/Application.h"
#include "Module/ModuleScene.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentLight.h"

EditorActionDeleteComponent::EditorActionDeleteComponent(Component* comp)
{
	UUID_COMP = comp->UUID;
	UUID_OWNER = comp->owner->UUID;
	comp->Save(serialization_component);
	type = comp->type;
}


void EditorActionDeleteComponent::Undo()
{
	Component* component;
	GameObject* owner = App->scene->GetGameObject(UUID_OWNER);
	component = owner->CreateComponent(type);
	component->Load(serialization_component);
}

void EditorActionDeleteComponent::Redo()
{
	Component* component = App->scene->GetComponent(UUID_COMP);
	GameObject* owner = App->scene->GetGameObject(UUID_OWNER);
	owner->RemoveComponent(component);
}
