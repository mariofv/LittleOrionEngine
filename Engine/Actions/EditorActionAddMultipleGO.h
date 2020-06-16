#ifndef _EDITORACTIONADDMULTIPLEGO_H_
#define _EDITORACTIONADDMULTIPLEGO_H_

#include "EditorAction.h"
#include "Helper/Config.h"
#include "EditorActionAddGameObject.h"

class GameObject;

class EditorActionAddMultipleGO : public EditorAction
{
public:
	EditorActionAddMultipleGO();
	~EditorActionAddMultipleGO() = default;

	void Undo();
	void Redo();
	bool HasParent(GameObject* go);
	bool BelongsToList(GameObject* go);

private:
	std::vector<EditorActionAddGameObject*> editors;
	std::vector<GameObject*> game_objects;
};

#endif _EDITORACTIONADDMULTIPLEGO_H_