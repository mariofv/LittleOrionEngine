#ifndef _COMPONENTMATERIAL_H_
#define _COMPONENTMATERIAL_H_

#include "Component.h"
#include "Globals.h"

#include <GL/glew.h>

class Texture;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	ComponentMaterial(GameObject * owner);
	~ComponentMaterial();

	void Enable() override;
	void Disable() override;
	void Update() override;

	GLuint GetTexture() const;

	void ShowComponentWindow() override;

public:
	ComponentType type = ComponentType::MATERIAL;

	int index = 0;
	Texture *texture;

	bool showCheckerboardTexture = false;
};

#endif //_COMPONENTMATERIAL_H_