#ifndef _COMPONENTIMAGE_H_
#define _COMPONENTIMAGE_H_

#include "Component/Component.h"

#include <GL/glew.h>

class Texture;

class ComponentImage : public Component
{
public:
	ComponentImage();
	ComponentImage(GameObject * owner);
	~ComponentImage();

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Delete() override;
	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	void SetTextureToRender(uint32_t texture_uuid);
	ENGINE_API void SetColor(float4 color);

	void Render(float4x4* projection);

	void SetNativeSize() const;

private:
	virtual void InitData();

public:
	uint32_t texture_uuid = 0;
	std::shared_ptr<Texture> texture_to_render;
	bool preserve_aspect_ratio = false;

	float4 color = float4::one;

private:
	GLuint program = 0;
	GLuint vao, vbo;

	float texture_aspect_ratio = 0.f;
};
#endif //_COMPONENTIMAGE_H_
