#ifndef __COMPONENTTRAIL_H_
#define __COMPONENTTRAIL_H_

#include "Component.h"

#include "MathGeoLib.h"
#include "Main/Application.h"
#include "Module/ModuleTime.h"
#include "Component/ComponentTransform.h"

#include <queue>
#include <list>

class GameObject;
class ComponentBillboard;
class ComponentTrailRenderer;
class ComponentTransform;

struct TrailPoint {
	float3 position;
	float3 position_perpendicular_point;
	float4 color;
	float width;
	float life;//tolal life of trail point
	float time_left;//remaining time of rendered trail point
	bool is_rendered = false;

	TrailPoint() {};

	TrailPoint(float3 position, float width, float life) : position(position), width(width), life(life), time_left(life) {}

	TrailPoint(const ComponentTransform& transform, float3 previous_point, float width, float life) :position(position), width(width), life(life), time_left(life)
	{
		//float3 vector_adjacent = (previous_point - transform.GetGlobalTranslation()).Normalized();//vector between each pair -> Normalized to get vector with magnitutde = 1 but same direction
		//position_perpendicular_point = vector_adjacent.Cross(transform.GetFrontVector());
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
	void RespawnTrailPoint(TrailPoint& point);

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

public:
	uint32_t texture_uuid = 0;
	ComponentTrailRenderer* trail_renderer = nullptr;
	std::queue<TrailPoint> trail_points;
	float3 gameobject_init_position = { 0.0f, 0.0f, 0.0f};

	int total_points = 1;
	float3 last_point_added;
	//Trail Generation properties
	float width = 1.0f;
	float duration = 1000.0f; // in millis
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

	//Toni tests stuff
	std::vector<TrailPoint> test_points; // These are individual points that define the path
	TrailPoint head_point;
	std::vector <float3> points_to_delete;
	TrailPoint last_point;
	std::vector <std::pair <TrailPoint*, TrailPoint*>> mesh_points; // These are from which we're gonna build the mesh
	void GetPerpendiculars();
	
};

#endif