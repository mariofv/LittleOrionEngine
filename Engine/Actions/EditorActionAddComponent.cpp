#include "EditorActionAddComponent.h"
#include "Main/GameObject.h"
#include "Main/Application.h"
#include "Module/ModuleScene.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentMesh.h"
#include "Component/ComponentLight.h"

EditorActionAddComponent::EditorActionAddComponent(Component * comp)
{
	UUID_COMP = comp->UUID;
	UUID_OWNER = comp->owner->UUID;
	comp->Save(serialization_component);
	type = comp->type;
}

void EditorActionAddComponent::Undo()
{
	Component* component = App->scene->GetComponent(UUID_COMP);
	GameObject* owner = App->scene->GetGameObject(UUID_OWNER);
	owner->RemoveComponent(component);
}

void EditorActionAddComponent::Redo()
{
	Component* component;
	GameObject* owner = App->scene->GetGameObject(UUID_OWNER);
	component = owner->CreateComponent(type);
	component->Load(serialization_component);
}
