#ifndef _COMPONENTTEXT_H_
#define _COMPONENTTEXT_H_

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "Component.h"

#include <string>
#include <GL/glew.h>

class Font;
class Quad;

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
	~ComponentText();

	Component* Clone(GameObject* owner, bool original_prefab) override;
	void CopyTo(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	void InitResource(uint32_t uuid, ResourceType resource) override;

	void ReassignResource() override;

	void Update() override;
	void Render(float4x4* projection);

	ENGINE_API void SetText(const std::string& new_text);
	void SetHorizontalAlignment(HorizontalAlignment horizontal_alignment);

	void SetFont(uint32_t font_uuid);
	ENGINE_API void SetFontSize(float font_size);

	ENGINE_API void SetFontColor(const float4& new_color);
	ENGINE_API float4 GetFontColor() const;

private:
	void InitData();

	void ComputeTextLines();
	float GetLineStartPosition(float line_size) const;

public:
	Quad* text_quad = nullptr;

	//Text Inputs
	std::string text = "Default";
	HorizontalAlignment horizontal_alignment = HorizontalAlignment::LEFT;

	uint32_t font_uuid = 0;
	std::shared_ptr<Font> font = nullptr;

	float font_size = 12.0f;
	float4 font_color = float4::one;

	float text_width = 0;
	float text_heigth = 0;
	float pacing = 20.f;

private:
	GLuint program = 0;
	GLuint vao, vbo, ebo;

	float scale_factor = 0.f;
	std::vector<float> line_sizes;
	bool is_jump_line = false;
};
#endif