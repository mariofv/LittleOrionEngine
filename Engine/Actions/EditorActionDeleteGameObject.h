#ifndef _EDITORACTIONDELETEGAMEOBJECT_H_
#define _EDITORACTIONDELETEGAMEOBJECT_H_

#include "EditorAction.h"
#include "GameObject.h"

class EditorActionDeleteGameObject : public EditorAction
{
public:
	EditorActionDeleteGameObject(GameObject* GO, GameObject* parentGO, int depth);
	~EditorActionDeleteGameObject();

	void Undo();
	void Redo();

private:
	GameObject* game_object = nullptr;
	GameObject* parent = nullptr;
	int hierarchy_depth = 0;
};

#endif _EDITORACTIONDELETEGAMEOBJECT_H_