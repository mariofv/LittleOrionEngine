#include "EditorActionAddMultipleGO.h"

#include "EditorActionAddGameObject.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"


EditorActionAddMultipleGO::EditorActionAddMultipleGO()
{
	for (auto go : App->editor->selected_game_objects) 
	{

		game_objects.push_back(go);
	}
	for (auto go : App->editor->selected_game_objects) 
	{

		if (!App->scene->HasParent(go))
		{
			EditorActionAddGameObject *del = new EditorActionAddGameObject(go);
			editors.push_back(del);
		}
	}
}

void EditorActionAddMultipleGO::Undo()
{
	for (auto editor : editors) 
	{
		editor->Undo();
	}
}

void EditorActionAddMultipleGO::Redo()
{
	for (auto editor : editors) 
	{
		editor->Redo();
	}
}