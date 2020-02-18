#ifndef _EDITORACTIONENABLEDISABLECOMPONENT_H_
#define _EDITORACTIONENABLEDISABLECOMPONENT_H_

#include "EditorAction.h"
#include "Component/Component.h"


class EditorActionEnableDisableComponent : public EditorAction
{
public:
	EditorActionEnableDisableComponent(Component* comp);
	~EditorActionEnableDisableComponent() = default;

	void Undo();
	void Redo();

private:
	uint64_t UUID_COMP;
};


#endif _EDITORACTIONENABLEDISABLECOMPONENT_H_
