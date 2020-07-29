#include "EditorActionMultipleScale.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"



EditorActionMultipleScale::EditorActionMultipleScale(float3 &previous, float3 &current) : previous_scale(previous),
current_scale(current)
{

	for (auto go : App->editor->selected_game_objects)
	{
		if (!HasParent(go))
		{
			game_objects_UUID.push_back(go->UUID);
		}
	}
	scale_factor = current.Div(previous);
}


void EditorActionMultipleScale::Undo()
{
	for (auto UUID_go : game_objects_UUID)
	{
		GameObject* go = App->scene->GetGameObject(UUID_go);
		go->transform.SetScale(go->transform.GetScale().Mul(float3::one.Div(scale_factor)));
	}
	return;
}

void EditorActionMultipleScale::Redo()
{
	for (auto UUID_go : game_objects_UUID)
	{
		GameObject* go = App->scene->GetGameObject(UUID_go);
		go->transform.SetScale(go->transform.GetScale().Mul(scale_factor));
	}
	return;
}

bool EditorActionMultipleScale::HasParent(GameObject * go) const
{
	if (go->GetHierarchyDepth() == 1)
	{
		return false;
	}

	int depth = go->GetHierarchyDepth();

	GameObject *game_object = go;

	while (depth >= 2) {
		if (BelongsToList(game_object->parent))
		{
			return true;

		}
		game_object = game_object->parent;
		depth = depth - 1;
	}
	return false;
}

bool EditorActionMultipleScale::BelongsToList(GameObject * game_object) const
{
	for (auto go : App->editor->selected_game_objects)
	{
		if (go->UUID == game_object->UUID)
		{
			return true;
		}
	}
	return false;
}
