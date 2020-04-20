#ifndef _COMPONENTBUTTON_H_
#define _COMPONENTBUTTON_H_

#include "ComponentUI.h"
class ComponentButton : public ComponentUI
{
public:
	ComponentButton();
	ComponentButton(GameObject * owner);

	~ComponentButton() = default;

	void Delete() override;

	void UISpecializedSave(Config& config) const override;
	void UISpecializedLoad(const Config& config) override;

	void Render(float4x4* projection);

private:
	bool hasText = false;
};
#endif

