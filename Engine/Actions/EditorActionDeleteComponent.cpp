#include "EditorActionDeleteComponent.h"
#include "Main/GameObject.h"
#include "Main/Application.h"
#include "Module/ModuleScene.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentLight.h"

EditorActionDeleteComponent::EditorActionDeleteComponent(Component* comp) : component_UUID(comp->UUID),
owner_UUID(comp->owner->UUID), type(comp->type)
{
	comp->Save(serialization_component);
}


void EditorActionDeleteComponent::Undo()
{
	Component* component;
	GameObject* owner = App->scene->GetGameObject(owner_UUID);
	component = owner->CreateComponent(type);
	component->Load(serialization_component);
}

void EditorActionDeleteComponent::Redo()
{
	Component* component = App->scene->GetComponent(component_UUID);
	GameObject* owner = App->scene->GetGameObject(owner_UUID);
	owner->RemoveComponent(component);
}
