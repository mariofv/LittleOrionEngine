#ifndef _EDITORACTIONADDGAMEOBJECT_H_
#define _EDITORACTIONADDGAMEOBJECT_H_

#include "EditorAction.h"
#include "Helper/Config.h"

class GameObject;

class EditorActionAddGameObject : public EditorAction
{
public:
	EditorActionAddGameObject(GameObject* go);
	~EditorActionAddGameObject() = default;

	void Undo();
	void Redo();

private:
	GameObject* game_object;
	Config serialization_gameobject;
};

#endif _EDITORACTIONADDGAMEOBJECT_H_