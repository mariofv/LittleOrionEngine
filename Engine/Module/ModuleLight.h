#ifndef _MODULELIGHT_H_
#define _MODULELIGHT_H_
#include "Module.h"

class ComponentLight;

class ModuleLight : public Module
{
public:
	ModuleLight() = default;
	~ModuleLight();

	bool Init() override;
	bool CleanUp() override;

	ComponentLight* CreateComponentLight();
	void RemoveComponentLight(ComponentLight* light_to_remove);

public:
	std::vector<ComponentLight*> lights;
};

#endif // !_MODULELIGHT_H_

