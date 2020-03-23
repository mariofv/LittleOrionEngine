#include "EditorActionTranslate2D.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"

EditorActionTranslate2D::EditorActionTranslate2D(ComponentTransform2D* component, float2 &current, GameObject* go) : previous_position(component->position),
current_position(current), UUID_go(go->UUID) {}


void EditorActionTranslate2D::Undo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform_2d.position = previous_position;
	return;
}

void EditorActionTranslate2D::Redo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform_2d.position = current_position;
	return;
}

