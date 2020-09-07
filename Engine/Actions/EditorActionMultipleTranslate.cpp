#include "EditorActionMultipleTranslate.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"


EditorActionMultipleTranslate::EditorActionMultipleTranslate(const float3 &previous, const float3 &current): previous_position(previous),
current_position(current)
{

	for (auto go : App->editor->selected_game_objects)
	{
		if (!App->scene->HasParent(go))
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
