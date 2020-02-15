#ifndef _EDITORACTIONDELETECOMPONENT_H_
#define _EDITORACTIONDELETECOMPONENT_H_

#include "EditorAction.h"
#include "Component/Component.h"

class EditorActionDeleteComponent : public EditorAction
{
public:
	EditorActionDeleteComponent(Component* comp);
	~EditorActionDeleteComponent();

	void Undo();
	void Redo();

private:
	Component* component;
};

#endif _EDITORACTIONDELETECOMPONENT_H_