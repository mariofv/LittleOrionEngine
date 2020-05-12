#ifndef _COMPONENTTEXT_H_
#define _COMPONENTTEXT_H_

#include "Component.h"

#include <string>
#include <GL/glew.h>

class Font;

class ComponentText : public Component
{
public:
	enum class HorizontalAlignment
	{
		LEFT,
		CENTER,
		RIGHT
	};

	ComponentText();
	ComponentText(GameObject * owner);
	~ComponentText() = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	void Update() override;
	void Render(float4x4* projection);

	void SetText(const std::string& new_text);
	void SetFont(uint32_t font_uuid);
	void SetFontSize(int font_size);

private:
	void InitData();

	void ComputeTextLines();
	float GetLineStartPosition(float line_size) const;

public:
	//Text Inputs
	std::string text = "Default";
	HorizontalAlignment horizontal_alignment = HorizontalAlignment::LEFT;

	uint32_t font_uuid = 0;
	std::shared_ptr<Font> font = nullptr;

	float font_size = 12.0f;
	float3 font_color = float3::one;

	float text_width = 0;
	float text_heigth = 0;

private:
	GLuint program, vao, vbo;

	float scale_factor = 0.f;
	std::vector<float> line_sizes;
};
#endif