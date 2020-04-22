#include "EditorActionAddGameObject.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleRender.h"

EditorActionAddGameObject::EditorActionAddGameObject(GameObject * go) : game_object(go)
{
	go->Save(serialization_gameobject);
}

void EditorActionAddGameObject::Undo()
{
	App->scene->RemoveGameObject(game_object);
}

void EditorActionAddGameObject::Redo()
{
	GameObject* created_game_object = App->scene->CreateGameObject();
	created_game_object->Load(serialization_gameobject);
	game_object = created_game_object;
}
