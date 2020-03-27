#ifndef _COMPONENTTEXT_H_
#define _COMPONENTTEXT_H_

#include "Component.h"
// Std. Includes
#include <iostream>
#include <map>
#include <string>
// GLEW
#include <GL/glew.h>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H



class ComponentText : public Component
{
public:
	ComponentText();
	ComponentText(GameObject * owner);
	~ComponentText() = default;

	void Delete() override {};

	void Save(Config& config) const override {};
	void Load(const Config& config) override {};
	Component* Clone(bool original_prefab = false) const override { return nullptr; };
	void Copy(Component* component_to_copy) const override {};
	virtual void Render();
	float3 color = float3(0.0f, 0.0f, 1.0f);

	//Text Inputs
	std::string text = "Artemis & Marco Rule";
	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	GLfloat scale = 1.0f;

private:
	float2 position = float2(0.0f, 0.0f);
	float2 size = float2(10.0f, 10.0f);
	float4x4 model = float4x4::identity;
	
	float rotate = 0.0f;
	void InitData();
	friend class PanelComponent;
};
#endif