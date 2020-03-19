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

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	float2 Size;    // Size of glyph
	float2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class ComponentText : public Component
{
public:
	ComponentText();
	ComponentText(GameObject * owner);
	~ComponentText() = default;

	void Delete() override {};

	void Save(Config& config) const override {};
	void Load(const Config& config) override {};
	virtual void Render();
	float3 color = float3(0.0f, 1.0f, 0.0f);

	FT_Library ft;
	FT_Face face;
	
	std::map<GLchar, Character> Characters;

	GLuint VAO, VBO;
	//Text Inputs
	std::string text = "Default";
	GLfloat x = 25.0f;
	GLfloat y = 25.0f;
	GLfloat scale = 1.0f;

private:
	unsigned int shader_program, vao, text_texture;
	float2 position = float2(0.0f, 0.0f);
	float2 size = float2(10.0f, 10.0f);
	float4x4 model = float4x4::identity;
	float window_width, window_height;
	float rotate = 0.0f;
	void InitData();
	friend class PanelComponent;
};
#endif