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

	void Render(const float3& mesh_position, GLuint program);
	void RenderDirectionalLight(const float3& mesh_position);
	void RenderSpotLights(const float3& mesh_position, GLuint program);
	void RenderPointLights(const float3& mesh_position, GLuint program);

	ComponentLight* CreateComponentLight();
	void RemoveComponentLight(ComponentLight* light_to_remove);

private:
	void SortClosestLights(const float3& position, ComponentLight::LightType light_type);
	void SetDirectionalFrustum(); //SHADOWS
	void GenerateLightMatrices(GLuint program, bool render_depth);

public:
	static const unsigned int MAX_DIRECTIONAL_LIGHTS_RENDERED = 1;
	unsigned int current_number_directional_lights_rendered = 0;

	static const unsigned int MAX_SPOT_LIGHTS_RENDERED = 5;
	unsigned int current_number_spot_lights_rendered = 0;

	static const unsigned int MAX_POINT_LIGHTS_RENDERED = 5;
	unsigned int current_number_point_lights_rendered = 0;

	std::vector<ComponentLight*> lights;


	float4x4 directional_view; //FOR THE SHADOWS
	float4x4 directional_proj;
	Frustum light_frustum;


private:
	std::vector< std::pair<float, ComponentLight*> >  closest_lights;
	friend class ModuleEditor;
};

#endif // !_MODULELIGHT_H_