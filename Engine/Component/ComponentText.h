#ifndef _COMPONENTTEXT_H_
#define _COMPONENTTEXT_H_

#include "ComponentUI.h"
// Std. Includes
#include <iostream>
#include <map>
#include <string>
// GLEW
#include <GL/glew.h>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H



class ComponentText : public ComponentUI
{
public:
	ComponentText();
	ComponentText(GameObject * owner);
	~ComponentText() = default;

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	virtual void Render(float4x4*);

	//Text Inputs
	std::string text = "Default";
	GLfloat x = 0.0f;
	GLfloat y = 0.0f;
	GLfloat scale =  1.0f;


private:
	void InitData();
};
#endif