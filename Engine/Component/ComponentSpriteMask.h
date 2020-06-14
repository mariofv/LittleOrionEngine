#ifndef _COMPONENTSPRITEMASK_H_
#define _COMPONENTSPRITEMASK_H_

#include "Component/Component.h"

#include <GL/glew.h>

class Texture;

class ComponentSpriteMask : public Component
{
public:
	ComponentSpriteMask();
	ComponentSpriteMask(GameObject * owner);
	~ComponentSpriteMask();

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	void SetTextureToRender(uint32_t texture_uuid);

	void Render(float4x4* projection);

	void SetNativeSize();

private:
	virtual void InitData();

public:
	uint32_t texture_uuid = 0;
	std::shared_ptr<Texture> texture_to_render;
	float2 size;
	bool render_mask = false;
	bool inverted_mask = false;

private:
	GLuint program, vao, vbo;

	float texture_aspect_ratio = 0.f;
};
#endif //_COMPONENTSPRITEMASK_H_
