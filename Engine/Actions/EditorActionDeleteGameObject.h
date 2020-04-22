#ifndef _EDITORACTIONDELETEGAMEOBJECT_H_
#define _EDITORACTIONDELETEGAMEOBJECT_H_

#include "EditorAction.h"
#include "Helper/Config.h"

class GameObject;

class EditorActionDeleteGameObject : public EditorAction
{
public:
	EditorActionDeleteGameObject(GameObject* go);
	~EditorActionDeleteGameObject() = default;

	void Undo();
	void Redo();

private:
	Config serialized_gameobject;
	GameObject* game_object = nullptr;
	uint64_t UUID_go;
};

#endif _EDITORACTIONDELETEGAMEOBJECT_H_