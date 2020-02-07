#include "EditorActionAddGameObject.h"
#include "Module/ModuleScene.h"
#include "Application.h"


EditorActionAddGameObject::EditorActionAddGameObject(GameObject* go, GameObject* new_parent, int depth)
{
	game_object = go;
	parent = new_parent;
	depth = hierarchy_depth;
}


EditorActionAddGameObject::~EditorActionAddGameObject()
{
	if (!game_object->IsEnabled())
		App->scene->RemoveGameObject(game_object);
}

void EditorActionAddGameObject::Undo()
{
	game_object->SetEnabled(false);
	game_object->parent->RemoveChild(game_object);
	game_object->SetHierarchyDepth(0);
}

void EditorActionAddGameObject::Redo()
{
	game_object->SetEnabled(true);
	parent->children.push_back(game_object);
	game_object->parent = parent;
	game_object->SetHierarchyDepth(hierarchy_depth);

}
