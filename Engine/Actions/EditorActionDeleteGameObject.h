#ifndef _EDITORACTIONDELETEGAMEOBJECT_H_
#define _EDITORACTIONDELETEGAMEOBJECT_H_

#include "EditorAction.h"
#include "Main/GameObject.h"

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
	uint64_t UUID_GO;
};

#endif _EDITORACTIONDELETEGAMEOBJECT_H_