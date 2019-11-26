#ifndef _COMPONENTMATERIAL_H_
#define _COMPONENTMATERIAL_H_

#include "Component.h"
#include "Globals.h"

class Texture;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial() = default;
	~ComponentMaterial();

	void Enable() override;
	void Disable() override;
	void Update() override;

	ComponentType GetType() const override;

public:
	ComponentType type = ComponentType::MATERIAL;

	int index = 0;
	Texture *texture;
};

#endif //_COMPONENTMATERIAL_H_