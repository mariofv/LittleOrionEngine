#ifndef _EDITORACTIONADDCOMPONENT_H_
#define _EDITORACTIONADDCOMPONENT_H_

#include "EditorAction.h"
#include "Component/Component.h"

class EditorActionAddComponent : public EditorAction
{
public:
	EditorActionAddComponent(Component* comp);
	~EditorActionAddComponent() = default;

	void Undo();
	void Redo();

private:
	uint64_t UUID_COMP;
	uint64_t UUID_OWNER;
	Config serialization_component;
	Component::ComponentType type;
};

#endif _EDITORACTIONADDCOMPONENT_H_
