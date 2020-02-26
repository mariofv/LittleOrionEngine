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
	uint64_t component_UUID;
	uint64_t owner_UUID;
	Config serialization_component;
	Component::ComponentType type;
};

#endif _EDITORACTIONADDCOMPONENT_H_
