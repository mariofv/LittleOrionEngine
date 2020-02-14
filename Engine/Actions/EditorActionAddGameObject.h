#ifndef _EDITORACTIONADDGAMEOBJECT_H_
#define _EDITORACTIONADDGAMEOBJECT_H_

#include "EditorAction.h"
#include "Main/GameObject.h"


class EditorActionAddGameObject : public EditorAction
{
public:
	EditorActionAddGameObject(GameObject* go, GameObject* par, int depth) : game_object(go), parent(par), hierarchy_depth(depth) {};
	~EditorActionAddGameObject();

	void Undo();
	void Redo();

private:
	GameObject* game_object;
	GameObject* parent;
	int hierarchy_depth = 0;
};

#endif _EDITORACTIONADDGAMEOBJECT_H_