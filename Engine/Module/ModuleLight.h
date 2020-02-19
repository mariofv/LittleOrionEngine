#ifndef _MODULELIGHT_H_
#define _MODULELIGHT_H_

#include "Module.h"

#include <vector>
#include <MathGeoLib.h>
#include <GL/glew.h>
class ComponentLight;

class ModuleLight : public Module
{
public:
	struct PointLightParameters
	{
		std::vector<float4> light_positions;
		std::vector<float4> light_colors;

		std::vector<float> light_constants;
		std::vector<float> light_quadratics;
		std::vector<float> light_linears;
	};

	ModuleLight() = default;
	~ModuleLight();

	bool Init() override;
	bool CleanUp() override;

	void Render(GLuint program);
	void RenderDirectionalLight();
	void RenderSpotLight(GLuint program);
	void RenderPointLight(GLuint program);
	void RenderLights();
	void RenderDarkness() const;

	ComponentLight* CreateComponentLight();
	void RemoveComponentLight(ComponentLight* light_to_remove);

public:
	static const unsigned int MAX_DIRECTIONAL_LIGHTS_RENDERED = 1;
	int current_number_directional_lights_rendered = 0;

	static const unsigned int MAX_SPOT_LIGHTS_RENDERED = 5;
	int current_number_spot_lights_rendered = 0;

	static const unsigned int MAX_POINT_LIGHTS_RENDERED = 5;
	int current_number_point_lights_rendered = 0;

	std::vector<ComponentLight*> lights;
	friend class ModuleEditor;

private:
	std::vector<float4> light_positions;
	std::vector<float4> light_colors;
};

#endif // !_MODULELIGHT_H_

