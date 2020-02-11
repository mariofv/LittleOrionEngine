#ifndef _MODULELIGHT_H_
#define _MODULELIGHT_H_
#include "Module.h"

class ComponentLight;

class ModuleLight : public Module
{
public:
	ModuleLight() = default;
	~ModuleLight();

	bool CleanUp() override;
	void RenderLight() const;

	ComponentLight* CreateComponentLight();
	void RemoveComponentLight(ComponentLight* light_to_remove);

	void RenderDarkness() const;

public:
	std::vector<ComponentLight*> lights;
	friend class ModuleEditor;
};

#endif // !_MODULELIGHT_H_

