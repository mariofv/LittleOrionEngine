#include "EditorActionDeleteGameObject.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleRender.h"
#include "Main/Application.h"


EditorActionDeleteGameObject::~EditorActionDeleteGameObject()
{
	if(!game_object->IsEnabled())
		App->scene->RemoveGameObject(game_object);
}

void EditorActionDeleteGameObject::Undo()
{
	game_object->SetEnabled(true);
	parent->children.push_back(game_object);
	game_object->parent = parent;
	game_object->SetHierarchyDepth(hierarchy_depth);
	if (!game_object->IsStatic())
		App->renderer->InsertAABBTree(game_object);
}

void EditorActionDeleteGameObject::Redo()
{
	game_object->SetEnabled(false);
	game_object->parent->RemoveChild(game_object);
	game_object->SetHierarchyDepth(0);
	if (!game_object->IsStatic())
		App->renderer->RemoveAABBTree(game_object);
}

