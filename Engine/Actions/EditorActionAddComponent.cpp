#include "EditorActionAddComponent.h"
#include "Main/GameObject.h"
#include "Main/Application.h"
#include "Module/ModuleScene.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentLight.h"

EditorActionAddComponent::EditorActionAddComponent(Component * comp) : component_UUID(comp->UUID),
owner_UUID(comp->owner->UUID), type(comp->type)

{
	comp->Save(serialization_component);
}

void EditorActionAddComponent::Undo()
{
	Component* component = App->scene->GetComponent(component_UUID);
	GameObject* owner = App->scene->GetGameObject(owner_UUID);
	owner->RemoveComponent(component);
	component->added_by_user = false;
}

void EditorActionAddComponent::Redo()
{
	Component* component;
	GameObject* owner = App->scene->GetGameObject(owner_UUID);
	component = owner->CreateComponent(type);
	component->Load(serialization_component);
	component->added_by_user = true;
}
