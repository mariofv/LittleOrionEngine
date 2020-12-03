#include "EditorActionDeleteMultipleGO.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"

EditorActionDeleteMultipleGO::EditorActionDeleteMultipleGO()
{
	for (auto go : App->editor->selected_game_objects) 
	{

		game_objects.push_back(go);
	}
	for (auto go : App->editor->selected_game_objects) 
	{
		
		if (!App->scene->HasParent(go))
		{
			EditorActionDeleteGameObject *del = new EditorActionDeleteGameObject(go);
			editors.push_back(del);
		}
	}
}

void EditorActionDeleteMultipleGO::Undo()
{
	for (auto editor : editors) 
	{
		editor->Undo();
	}
}

void EditorActionDeleteMultipleGO::Redo()
{
	for (auto editor : editors) 
	{
		editor->Redo();
	}
}
