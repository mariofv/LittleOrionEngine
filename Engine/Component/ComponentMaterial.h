#ifndef _COMPONENTMATERIAL_H_
#define _COMPONENTMATERIAL_H_

#include "Component.h"
#include "UI/ComponentsUI.h"
#include "Globals.h"

#include <GL/glew.h>
#include <memory>

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
	void Delete() override;

	GLuint GetTexture() const;

	void ShowComponentWindow() override;

public:
	ComponentType type = ComponentType::MATERIAL;

	int index = 0;
	std::shared_ptr<Texture> texture;

	bool show_checkerboard_texture = false;

	friend void ComponentsUI::ShowComponentMaterialWindow(ComponentMaterial *material);
};

#endif //_COMPONENTMATERIAL_H_