#ifndef __COMPONENTTRAIL_H_
#define __COMPONENTTRAIL_H_

#include "Component.h"

#include "MathGeoLib.h"
#include "Main/Application.h"
#include "Module/ModuleTime.h"

#include <queue>

class GameObject;
class ComponentBillboard;

struct TrailPoint {
	float3 position;
	float3 position_perpendicular_point;
	float4 color;
	float width;
	float life;//tolal life of trail point
	float time_left;//remaining time of rendered trail point
	bool is_rendered = false;

	TrailPoint(float3 position, float width, float life) : position(position), width(width), life(life), time_left(life) {}

	TrailPoint(float3 position, float3 previous_point, float width, float life) :position(position), width(width), life(life), time_left(life)
	{
		float3 cross = (previous_point - position).Normalized(); //perpendicular vector point calculated between Previous Point & Current Point -> Normalized to get vector with magnitutde = 1 but same direction
		position_perpendicular_point = cross;
		is_rendered = true;
	}
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
	std::queue<TrailPoint> trail_points;
	float3 gameobject_init_position = { 0.0f, 0.0f, 0.0f};

	int total_points = 1;
	float3 last_point_added;
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
	bool on_transform_change = false;
	//time
	float time_counter = 0.0f;
};

#endif