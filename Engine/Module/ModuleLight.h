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

	void RenderDirectionalLight(const ComponentLight& light);
	void RenderSpotLights(const ComponentLight& light, GLuint program);
	void RenderPointLights(const ComponentLight& light, GLuint program);

	void UpdateLightAABB(GameObject& object_aabb);
	void RecordShadowsFrameBuffers(int width, int height);

	ComponentLight* CreateComponentLight();
	void RemoveComponentLight(ComponentLight* light_to_remove);

private:
	void UpdateDirectionalLightFrustums(float3 max, float3 min);

	void SetDirectionalLightFrustums();
	void SendShadowUniformsToShader(GLuint program);

	void SortClosestLights(const float3& position);


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
	bool render_shadows = true;
	float main_camera_fov_increment_factor = 2;

private:
	Quat directional_light_rotation;

	//Directional light frustums - Generate shadows at different resolution
	ComponentCamera* directional_light_camera = nullptr;
	ComponentCamera* directional_light_mid = nullptr;
	ComponentCamera* directional_light_far = nullptr;

	GameObject*	dir_light_game_object = nullptr;
	GameObject*	dir_light_game_object_mid = nullptr;
	GameObject*	dir_light_game_object_far = nullptr;

	bool rendering_shadows = false;

	friend class ModuleEditor;
};

#endif // !_MODULELIGHT_H_