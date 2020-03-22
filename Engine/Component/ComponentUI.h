#ifndef _COMPONENTUI_H_
#define _COMPONENTUI_H_

#include "Component.h"

class ComponentUI : public Component
{
public:
	ComponentUI();
	ComponentUI(GameObject * owner);
	~ComponentUI() = default;

	void Delete() override {};

	void Save(Config& config) const override {};
	void Load(const Config& config) override {};
	Component* Clone(bool original_prefab = false) const override { return nullptr; };
	void Copy(Component* component_to_copy) const override {};
	virtual void Render();
	float3 color = float3(0.0f, 1.0f, 0.0f);
private:
	unsigned int shader_program, vao, ui_texture;
	float2 position = float2(0.0f, 0.0f);
	float2 size = float2(10.0f, 10.0f);
	float4x4 model = float4x4::identity;
	float window_width, window_height;
	float rotate = 0.0f;
	void InitData();
	friend class PanelComponent;
};
#endif
