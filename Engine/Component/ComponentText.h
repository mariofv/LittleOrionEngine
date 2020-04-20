#ifndef _COMPONENTTEXT_H_
#define _COMPONENTTEXT_H_

#include "ComponentUI.h"

#include <string>

class Font;

class ComponentText : public ComponentUI
{
public:
	ComponentText();
	ComponentText(GameObject * owner);
	~ComponentText() = default;

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	void Render(float4x4* projection);

	void SetFont(uint32_t font_uuid);

private:
	void InitData();

public:
	//Text Inputs
	std::string text = "Default";
	float scale = 12.0f;
	float text_width = 0;
	float text_heigth = 0;

public:
	uint32_t font_uuid = 0;
	std::shared_ptr<Font> font = nullptr;

};
#endif