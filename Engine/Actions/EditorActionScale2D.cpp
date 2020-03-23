#include "EditorActionScale2D.h"
#include "Module/ModuleScene.h"
#include "Main/Application.h"


EditorActionScale2D::EditorActionScale2D(ComponentTransform2D* component, float2 &current, GameObject* go) : previous_scale(component->size),
current_scale(current), UUID_go(go->UUID) {}

void EditorActionScale2D::Undo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform_2d.size = previous_scale;
	return;
}

void EditorActionScale2D::Redo()
{
	GameObject* action_GO = App->scene->GetGameObject(UUID_go);
	action_GO->transform_2d.size = current_scale;
	return;
}