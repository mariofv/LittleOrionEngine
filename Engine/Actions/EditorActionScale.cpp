#include "EditorActionScale.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleScene.h"

EditorActionScale::EditorActionScale(const float3 &previous, const float3 &current, GameObject* go) : previous_scale(previous),
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