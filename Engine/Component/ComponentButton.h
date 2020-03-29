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

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	virtual void Render(float4x4*);
private:
	void InitData();

};
#endif

