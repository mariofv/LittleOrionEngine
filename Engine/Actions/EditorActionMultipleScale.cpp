#include "EditorActionMultipleScale.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleActions.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"


EditorActionMultipleScale::EditorActionMultipleScale(const float3 &previous, const float3 &current) : previous_scale(previous),
current_scale(current)
{

	for (auto go : App->editor->selected_game_objects)
	{
		if (!App->scene->HasParent(go))
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