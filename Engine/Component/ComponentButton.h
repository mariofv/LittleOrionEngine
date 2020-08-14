#ifndef _COMPONENTBUTTON_H_
#define _COMPONENTBUTTON_H_

#include "Component.h"

class ComponentButton : public Component
{
public:
	ComponentButton();
	ComponentButton(GameObject * owner);

	~ComponentButton() = default;

	void PostUpdate() override;

	Component* Clone(GameObject* owner, bool original_prefab) override;
	void CopyTo(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	void SetClicked(bool clicked);
	ENGINE_API bool IsClicked() const;

private:
	bool clicked = false;
};
#endif

