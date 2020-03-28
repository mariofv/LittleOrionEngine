#ifndef _COMPONENTUI_H_
#define _COMPONENTUI_H_

#include "Component.h"

class ComponentUI : public Component
{
public:
	enum class UIType {
		CANVAS,
		IMAGE,
		TEXT
	};
	ComponentUI();
	ComponentUI(GameObject * owner);
	~ComponentUI() = default;

	virtual void Delete();

	virtual void Save(Config& config) const;
	virtual void Load(const Config& config);
	virtual Component* Clone(bool original_prefab = false) const { return nullptr; };
	virtual void Copy(Component* component_to_copy) const {};
	virtual void Render(float4x4*);
	float3 color = float3(0.0f, 1.0f, 0.0f);
private:
	unsigned int shader_program, vao, vbo, ui_texture;
	void InitData();
	friend class PanelComponent;
};
#endif
