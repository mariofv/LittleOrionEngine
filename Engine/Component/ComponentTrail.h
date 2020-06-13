#ifndef __COMPONENTTRAIL_H_
#define __COMPONENTTRAIL_H_

#include "Component.h"

#include "MathGeoLib.h"
#include "Main/Application.h"
#include "Module/ModuleTime.h"

class GameObject;
class ComponentBillboard;

struct TrailPoint {
	float3 position;
	float3 position_adjacent_point;
	float4 color;
	float width;
	float life;
	float time_passed;
	bool is_rendered = false;
};

class ComponentTrail : public Component
{
public:
	ComponentTrail();
	~ComponentTrail();

	ComponentTrail(GameObject* owner);


	void Init();

	ComponentTrail & operator=(const ComponentTrail& component_to_copy);
	ComponentTrail & operator=(ComponentTrail&& component_to_move) = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;
	void Delete() override;

	void UpdateTrail();
	void Render();
	void SetTrailTexture(uint32_t texture_uuid);

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

public:
	uint32_t texture_uuid = 0;
	ComponentBillboard* billboard = nullptr;
	std::vector<TrailPoint> trail_points;
	int total_points = 1;

	//Trail Generation properties
	float width = 10.0f;
	float duration = 0.5f;
	float min_distance = 1.0f;

	//Color properties
	float color_trail[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool fade = false;
	float fade_time = 1.0F;
	float color_fade_time = 1.0F;
	bool fade_between_colors = false;
	float color_to_fade[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//Render properties
	int blend_mode; //0 for aplha blend, 1 for addition
	unsigned int trail_vertices;
	
	//Standard parameters
	bool active = true;

	//time
	float time_counter = 0.0f;
};

#endif