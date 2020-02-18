#ifndef _MODULELIGHT_H_
#define _MODULELIGHT_H_

#include "Module.h"

#include <vector>
#include <MathGeoLib.h>

class ComponentLight;

class ModuleLight : public Module
{
public:
	ModuleLight() = default;
	~ModuleLight();

	bool Init() override;
	bool CleanUp() override;

	void Render();
	void RenderLights();
	void RenderDarkness() const;

	ComponentLight* CreateComponentLight();
	void RemoveComponentLight(ComponentLight* light_to_remove);

public:
	static const unsigned int MAX_LIGHTS_RENDERED = 10;
	int current_lights_rendered = 0;

	std::vector<ComponentLight*> lights;
	friend class ModuleEditor;

private:
	std::vector<float4> light_positions;
	std::vector<float4> light_colors;
};

#endif // !_MODULELIGHT_H_

