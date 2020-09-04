#include "EditorActionMultipleRotation.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"



EditorActionMultipleRotate::EditorActionMultipleRotate(const float3 &previous, const float3 &current) : previous_rotation(previous),
current_rotation(current)
{

	for (auto go : App->editor->selected_game_objects)
	{
		if (!App->scene->HasParent(go))
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