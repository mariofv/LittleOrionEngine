#ifndef _COMPONENTUI_H_
#define _COMPONENTUI_H_

#include "Component.h"

class ComponentUI : public Component
{
public:
	ComponentUI(ComponentType ui_type);
	ComponentUI(GameObject * owner, ComponentType ui_type);
	virtual ~ComponentUI();

	virtual void Delete();

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	virtual void UISpecializedSave(Config& config) const = 0;
	virtual void UISpecializedLoad(const Config& config) = 0;

	virtual Component* Clone(bool original_prefab = false) const { return nullptr; };
	virtual void Copy(Component* component_to_copy) const {};

	virtual void Enable() override;
	virtual void Disable() override;

public:
	int layer = 0; 
	friend class PanelComponent;
};
#endif
