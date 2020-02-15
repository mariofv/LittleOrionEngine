#ifndef _EDITORACTIONENABLEDISABLECOMPONENT_H_
#define _EDITORACTIONENABLEDISABLECOMPONENT_H_

#include "EditorAction.h"
#include "Component/Component.h"


class EditorActionEnableDisableComponent : public EditorAction
{
public:
	EditorActionEnableDisableComponent(Component* comp);
	~EditorActionEnableDisableComponent();

	void Undo();
	void Redo();

private:
	Component* component;

};


#endif _EDITORACTIONENABLEDISABLECOMPONENT_H_
