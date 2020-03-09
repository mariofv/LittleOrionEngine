#include "EditorActionRotation.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionRotation::EditorActionRotation(float3 &previous, float3 &current, GameObject* go) : previous_rotation(previous),
current_rotation(current), UUID_go(go->UUID) {}

void EditorActionRotation::Undo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->GetTransform()->SetRotation(previous_rotation);
	return;
}

void EditorActionRotation::Redo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->GetTransform()->SetRotation(current_rotation);
	return;
}