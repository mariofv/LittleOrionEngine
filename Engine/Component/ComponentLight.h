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
};

#endif // !_COMPONENTLIGHT_H

