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
	void RenderDirectionalLight(const float3& mesh_position);
	void RenderSpotLights(const float3& mesh_position, GLuint program);
	void RenderPointLights(const float3& mesh_position, GLuint program);
	void UpdateLightAABB(GameObject& object_aabb);

	ComponentLight* CreateComponentLight();
	void RemoveComponentLight(ComponentLight* light_to_remove);

private:
	void SortClosestLights(const float3& position, ComponentLight::LightType light_type);
	void SendShadowMatricesToShader(GLuint program);

public:
	static const unsigned int MAX_DIRECTIONAL_LIGHTS_RENDERED = 1;
	unsigned int current_number_directional_lights_rendered = 0;

	static const unsigned int MAX_SPOT_LIGHTS_RENDERED = 5;
	unsigned int current_number_spot_lights_rendered = 0;

	static const unsigned int MAX_POINT_LIGHTS_RENDERED = 5;
	unsigned int current_number_point_lights_rendered = 0;

	std::vector<ComponentLight*> lights;

	float ambient_light_intensity = 0.3;
	float ambient_light_color[4] = { 1, 1, 1, 1 };
	
	AABB   light_aabb;
	float3 light_position = float3::zero;
	
	OBB light_obb;
	OBB object_obb;

private:
	std::vector< std::pair<float, ComponentLight*> >  closest_lights;
	friend class ModuleEditor;
	Quat directional_light_rotation;

};

#endif // !_MODULELIGHT_H_