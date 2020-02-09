#ifndef _EDITORACTIONADDCOMPONENT_H_
#define _EDITORACTIONADDCOMPONENT_H_

#include "EditorAction.h"
#include "Component/Component.h"

class EditorActionAddComponent : public EditorAction
{
public:
	EditorActionAddComponent(Component* comp);
	~EditorActionAddComponent();

	void Undo();
	void Redo();

private:
	Component* component;
};

#endif _EDITORACTIONADDCOMPONENT_H_
