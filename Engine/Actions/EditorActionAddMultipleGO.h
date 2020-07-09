#ifndef _EDITORACTIONADDMULTIPLEGO_H_
#define _EDITORACTIONADDMULTIPLEGO_H_

#include "EditorAction.h"
#include <vector>

class GameObject;
class EditorActionAddGameObject;

class EditorActionAddMultipleGO : public EditorAction
{
public:
	EditorActionAddMultipleGO();
	~EditorActionAddMultipleGO() = default;

	void Undo();
	void Redo();
	bool HasParent(GameObject* go) const;
	bool BelongsToList(GameObject* go) const;

private:
	std::vector<EditorActionAddGameObject*> editors;
	std::vector<GameObject*> game_objects;
};

#endif _EDITORACTIONADDMULTIPLEGO_H_