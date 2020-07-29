#include "EditorActionMultipleRotation.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"



EditorActionMultipleRotate::EditorActionMultipleRotate(float3 &previous, float3 &current) : previous_rotation(previous),
current_rotation(current)
{

	for (auto go : App->editor->selected_game_objects)
	{
		if (!HasParent(go))
		{
			game_objects_UUID.push_back(go->UUID);
		}
	}
	rotation_factor = current - previous;
}


void EditorActionMultipleRotate::Undo()
{
	for (auto UUID_go : game_objects_UUID)
	{
		GameObject* go = App->scene->GetGameObject(UUID_go);
		float3 aux = go->transform.GetRotationRadiants() - rotation_factor;
		go->transform.SetRotationRad(aux);
	}
	return;
}

void EditorActionMultipleRotate::Redo()
{
	for (auto UUID_go : game_objects_UUID)
	{
		GameObject* go = App->scene->GetGameObject(UUID_go);
		float3 aux = go->transform.GetRotationRadiants() + rotation_factor;
		go->transform.SetRotationRad(aux);
	}
	return;
}

bool EditorActionMultipleRotate::HasParent(GameObject * go) const
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

bool EditorActionMultipleRotate::BelongsToList(GameObject * game_object) const
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
