#include "EditorActionScale.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionScale::EditorActionScale(float3 &previous, float3 &current, GameObject* go) : previous_scale(previous),
current_scale(current), UUID_go(go->UUID) {}

void EditorActionScale::Undo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform.SetScale(previous_scale);
	return;
}

void EditorActionScale::Redo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform.SetScale(current_scale);
	return;
}