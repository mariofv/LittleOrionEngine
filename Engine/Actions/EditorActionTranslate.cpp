#include "EditorActionTranslate.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleScene.h"

EditorActionTranslate::EditorActionTranslate(float3 &previous, float3 &current, GameObject* go) : previous_position(previous),
current_position(current), UUID_go(go->UUID) {}


void EditorActionTranslate::Undo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform.SetTranslation(previous_position);
	return;
}

void EditorActionTranslate::Redo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform.SetTranslation(current_position);
	return;
}

