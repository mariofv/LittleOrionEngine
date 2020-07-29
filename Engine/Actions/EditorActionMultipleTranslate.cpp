#include "EditorActionMultipleTranslate.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"



EditorActionMultipleTranslate::EditorActionMultipleTranslate(float3 &previous, float3 &current): previous_position(previous),
current_position(current)
{

	for (auto go : App->editor->selected_game_objects)
	{
		if (!HasParent(go))
		{
			game_objects_UUID.push_back(go->UUID);
		}
	}
	translation_vector = current - previous;

}


void EditorActionMultipleTranslate::Undo()
{
	for (auto  UUID_go : game_objects_UUID)
	{
		GameObject* go = App->scene->GetGameObject(UUID_go);
		float3 trans = go->transform.GetTranslation();
		trans = trans - translation_vector;
		go->transform.SetTranslation(trans);
	}
	return;
}

void EditorActionMultipleTranslate::Redo()
{
	for (auto UUID_go : game_objects_UUID)
	{
		GameObject* go = App->scene->GetGameObject(UUID_go);
		float3 trans = go->transform.GetTranslation();
		trans = trans + translation_vector;
		go->transform.SetTranslation(trans);
	}
	return;
}

bool EditorActionMultipleTranslate::HasParent(GameObject * go) const
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

bool EditorActionMultipleTranslate::BelongsToList(GameObject * game_object) const
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
