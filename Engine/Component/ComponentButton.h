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
	void OnButtonChange();
	virtual void Render(float4x4*);
private:
	void InitData();
	float percentage = 20.0F;
	bool hasText = false;
};
#endif

