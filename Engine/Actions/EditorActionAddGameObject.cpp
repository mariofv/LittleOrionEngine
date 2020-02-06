#include "EditorActionAddGameObject.h"



EditorActionAddGameObject::EditorActionAddGameObject(GameObject* go, GameObject* new_parent, int depth)
{
	game_object = go;
	parent = new_parent;
	depth = hierarchy_depth;
}


EditorActionAddGameObject::~EditorActionAddGameObject()
{
}

void EditorActionAddGameObject::Undo()
{
	game_object->SetEnabled(true);
	parent->children.push_back(game_object);
	game_object->parent = parent;
	game_object->SetHierarchyDepth(hierarchy_depth);
}

void EditorActionAddGameObject::Redo()
{

}
