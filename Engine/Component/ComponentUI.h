#ifndef _COMPONENTUI_H_
#define _COMPONENTUI_H_

#include "Component.h"
class Texture;

class ComponentUI : public Component
{
public:
	ComponentUI(ComponentType ui_type);
	ComponentUI(GameObject * owner, ComponentType ui_type);
	virtual ~ComponentUI();

	virtual void Delete();

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	virtual void UISpecializedSave(Config& config) const = 0;
	virtual void UISpecializedLoad(const Config& config) = 0;

	virtual Component* Clone(bool original_prefab = false) const { return nullptr; };
	virtual void Copy(Component* component_to_copy) const {};

	virtual void Enable() override;
	virtual void Disable() override;

	virtual void Render(float4x4* projection);
	virtual void Render(float4x4* projection, float4x4* model, unsigned int texture = 0, float3* color = &float3(0.0f, 1.0f, 0.0f));
	virtual void InitData();

	void SetTextureToRender(uint32_t texture_uuid);

public:
	float3 color = float3::one;

	int layer = 0; 

	uint32_t texture_uuid = 0;
	std::shared_ptr<Texture> texture_to_render;
	std::string metadata_path;
	
protected:
	unsigned int shader_program, vao, vbo;
	unsigned int ui_texture = 2;
	friend class PanelComponent;
};
#endif
