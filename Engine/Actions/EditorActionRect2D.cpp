#include "EditorActionRect2D.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"

EditorActionRect2D::EditorActionRect2D(ComponentTransform2D* component, Rect &current, GameObject* go) : previous_rect(component->rect),
current_rect(current), UUID_go(go->UUID) {}


void EditorActionRect2D::Undo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform_2d.rect = previous_rect;
	return;
}

void EditorActionRect2D::Redo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform_2d.rect = current_rect;
	return;
}

