#include "EditorActionRotation2D.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionRotation2D::EditorActionRotation2D(ComponentTransform2D* component, float &current, GameObject* go) : previous_rotation(component->rotation),
current_rotation(current), UUID_go(go->UUID) {}

void EditorActionRotation2D::Undo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform_2d.rotation = previous_rotation;
	return;
}

void EditorActionRotation2D::Redo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform_2d.rotation = current_rotation;
	return;
}