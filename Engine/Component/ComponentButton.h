#ifndef _COMPONENTBUTTON_H_
#define _COMPONENTBUTTON_H_

#include "Component.h"

class ComponentButton : public Component
{
public:
	ComponentButton();
	ComponentButton(GameObject * owner);

	~ComponentButton() = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

private:
	bool hasText = false;
};
#endif

