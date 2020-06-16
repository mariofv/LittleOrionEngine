#include "EditorActionDeleteMultipleGO.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"

EditorActionDeleteMultipleGO::EditorActionDeleteMultipleGO()
{
	for (auto go : App->editor->selected_game_objects) {

		game_objects.push_back(go);
	}
	for (auto go : App->editor->selected_game_objects) {
		
		if (!HasParent(go)) {
			EditorActionDeleteGameObject *del = new EditorActionDeleteGameObject(go);
			editors.push_back(del);
		}
	}
}

void EditorActionDeleteMultipleGO::Undo()
{
	for (auto editor : editors) {
		editor->Undo();
	}
}

void EditorActionDeleteMultipleGO::Redo()
{
	for (auto editor : editors) {
		editor->Redo();
	}
}

bool EditorActionDeleteMultipleGO::HasParent(GameObject* go)
{
	if (go->GetHierarchyDepth() == 1) {
		return false;
	}

	int depth = go->GetHierarchyDepth();
	
	while (depth >= 2) {
		if (BelongsToList(go->parent)) {
			return true;

		}
		go = go->parent;
		depth = depth - 1;
	}
	return false;
}

bool EditorActionDeleteMultipleGO::BelongsToList(GameObject* game_object)
{
	for (auto go : game_objects)
	{
		if (go->UUID == game_object->UUID) {
			return true;
		}
	}
	return false;
}
