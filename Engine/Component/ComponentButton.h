#ifndef _COMPONENTBUTTON_H_
#define _COMPONENTBUTTON_H_

#include "ComponentUI.h"
class ComponentButton : public ComponentUI
{
public:
	ComponentButton();
	ComponentButton(GameObject * owner);

	~ComponentButton();

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;
	void Render(float4x4* projection);
private:
	bool hasText = false;
};
#endif

