#include "EditorActionRotation.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleScene.h"


EditorActionRotation::EditorActionRotation(float3 &previous, float3 &current, GameObject* go) : previous_rotation(previous),
current_rotation(current), UUID_go(go->UUID) {}

void EditorActionRotation::Undo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform.SetRotation(previous_rotation);
	return;
}

void EditorActionRotation::Redo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform.SetRotation(current_rotation);
	return;
}