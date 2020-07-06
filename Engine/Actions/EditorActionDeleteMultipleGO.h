#ifndef _EDITORACTIONDELETEMULTIPLEGO_H_
#define _EDITORACTIONDELETEMULTIPLEGO_H_

#include "EditorAction.h"
#include "Helper/Config.h"
#include "EditorActionDeleteGameObject.h"

class GameObject;

class EditorActionDeleteMultipleGO : public EditorAction
{
public:
	EditorActionDeleteMultipleGO();
	~EditorActionDeleteMultipleGO() = default;

	void Undo();
	void Redo();
	bool HasParent(GameObject* go) const;
	bool BelongsToList(GameObject* go) const;

private:

	std::vector<EditorActionDeleteGameObject*> editors;
	std::vector<GameObject*> game_objects;
};

#endif _EDITORACTIONDELETEMULTIPLEGO_H_
