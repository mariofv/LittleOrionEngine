#ifndef _COMPONENTTEXT_H_
#define _COMPONENTTEXT_H_

#include "ComponentUI.h"
#include <string>

class ComponentText : public ComponentUI
{
public:
	ComponentText();
	ComponentText(GameObject * owner);
	~ComponentText();

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	virtual void Render(float4x4*);

	//Text Inputs
	std::string text = "Default";
	float scale =  12.0f;
	float text_width = 0;
	float text_heigth = 0;

private:
	void InitData();
};
#endif