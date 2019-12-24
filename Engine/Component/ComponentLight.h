#ifndef _COMPONENTLIGHT_H
#define _COMPONENTLIGHT_H

#include "Component.h"

class GameObject;
class ComponentLight : public Component
{
public:
	ComponentLight();
	ComponentLight(GameObject * owner);
	~ComponentLight() = default;
	

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config &config) override;

	void ShowComponentWindow() override;

public:
	float light_color[3] = { 255.0f, 255.0f, 255.0f };
	float light_intensity = 1; 
};

#endif // !_COMPONENTLIGHT_H

