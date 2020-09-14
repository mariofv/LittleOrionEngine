#ifndef _MODULELIGHT_H_
#define _MODULELIGHT_H_

#include "Module.h"
#include "Component/ComponentLight.h"

#include <vector>
#include <MathGeoLib.h>
#include <GL/glew.h>

class ModuleLight : public Module
{
public:
	ModuleLight() = default;
	~ModuleLight();

	bool Init() override;
	bool CleanUp() override;
	update_status PostUpdate() override;

	void Render(const float3& mesh_position, GLuint program);

	ComponentLight* CreateComponentLight();
	void RemoveComponentLight(ComponentLight* light_to_remove);

private:
	void SendShadowUniformsToShader(GLuint program);

	void SortClosestLights(const float3& position);
	void RenderDirectionalLight(const ComponentLight& light);
	void RenderSpotLights(const ComponentLight& light, GLuint program);
	void RenderPointLights(const ComponentLight& light, GLuint program);

public:
	static const unsigned int MAX_DIRECTIONAL_LIGHTS_RENDERED = 1;
	unsigned int current_number_directional_lights_rendered = 0;

	static const unsigned int MAX_SPOT_LIGHTS_RENDERED = 5;
	unsigned int current_number_spot_lights_rendered = 0;

	static const unsigned int MAX_POINT_LIGHTS_RENDERED = 5;
	unsigned int current_number_point_lights_rendered = 0;

	std::vector<ComponentLight*> lights;

	AABB   light_aabb;
	float3 light_position = float3::zero;

	//Configurable values
	float ambient_light_intensity = 0.3f;
	float ambient_light_color[4] = { 1.f, 1.f, 1.f, 1.f };

	Quat directional_light_rotation;

	friend class ModuleEditor;
};

#endif // !_MODULELIGHT_H_