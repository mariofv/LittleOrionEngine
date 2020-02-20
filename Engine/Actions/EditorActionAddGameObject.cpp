#include "EditorActionAddGameObject.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleRender.h"
#include "Main/Application.h"



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
	App->editor->selected_game_object = nullptr;
	if(!game_object->IsStatic())
		App->renderer->RemoveAABBTree(game_object);
}

void EditorActionAddGameObject::Redo()
{
	game_object->SetEnabled(true);
	game_object->parent = parent;
	parent->children.push_back(game_object);
	game_object->SetHierarchyDepth(hierarchy_depth);
	App->editor->selected_game_object = game_object;
	if (!game_object->IsStatic())
		App->renderer->InsertAABBTree(game_object);
}
