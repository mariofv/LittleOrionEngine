#ifndef __COMPONENTTRAIL_H_
#define __COMPONENTTRAIL_H_

#include "Component.h"

#include "MathGeoLib.h"
#include "Main/Application.h"

#include <queue>
#include <list>

class GameObject;
class ComponentBillboard;
class ComponentTransform;
class Texture;

struct TrailPoint {
	float3 position;
	float4 color;
	float width;
	float life;//tolal life of trail point
	bool is_rendered = false;

	TrailPoint() {};

	TrailPoint(float3 position, float width, float life) : position(position), width(width), life(life) {}

	TrailPoint(const ComponentTransform& transform, float3 previous_point, float width, float life) :position(position), width(width), life(life)
	{
		is_rendered = true;
	}
};

struct Vertex
{
	float3 position;
	float2 uvs;
};

struct Spline
{
	std::vector<float3> spline_points;
	float3 GetSplinePoint(float t, float3 p0, float3 p1, float3 p2, float3 p3)
	{
		float t2 = t * t;
		float t3 = t * t*t;
		float3 v; // Interpolated point

		/* Catmull Rom spline Calculation */
		v = ((2 * p1) + (-p0 + p2) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 + (-p0 + 3 * p1 - 3 * p2 + p3) * t3) * 0.5;
		return v;
	}
};



class ComponentTrail : public Component
{
public:
	enum TextureMode
	{
		STRETCH,
		TILE,
		REPEAT_PER_SEGMENT
	};
	ComponentTrail();
	ComponentTrail(GameObject* owner);

	~ComponentTrail();

	void CleanUp();

	void Init();

	void InitBuffers();

	ComponentTrail & operator=(const ComponentTrail& component_to_copy) = default;
	ComponentTrail & operator=(ComponentTrail&& component_to_move) = default;

	Component* Clone(GameObject* owner, bool original_prefab) override;
	void CopyTo(Component* component_to_copy) const override;
	void Delete() override;

	void Update() override;

	void GetPerpendiculars();
	void GetCatmull();
	void CalculateCatmull(Spline& const path_to_smoothen, std::vector<float3>& spline_points);
	void GetUVs();

	void Render();
	void SetTrailTexture(uint32_t texture_uuid);
	void ChangeTexture(uint32_t texture_uuid);

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;

	void Disable() override;
	void Enable() override;

private:
	void ClearTrail();

public:
	uint32_t texture_uuid = 0;
	std::shared_ptr<Texture> trail_texture = nullptr;
	float3 last_gameobject_position;
	int total_points = 1;
	float3 last_point_added;
	//Trail generation properties
	float width = 1.0f;
	float duration = 1000.0f; // in millis
	float min_distance = 1.0f;


	bool active = true;
	std::vector<TrailPoint> test_points; // These are individual points that define the path
	TrailPoint last_point;
	std::vector <std::pair <TrailPoint*, TrailPoint*>> mesh_points; // These are from which we're gonna build the mesh

	//Renderer
	float* trail_renderer_vertices = nullptr;
	std::vector<Vertex> vertices;
	TextureMode texture_mode = ComponentTrail::TextureMode::STRETCH;
	int columns = 1, rows = 1;

	//Color properties
	float4 color = float4::one;
	float emissive_intensity = 1.f;
	bool blend_colors = false;
	float blend_percentage = 0.5f;
	float smoothening_step = 0.3f;
	float4 color_to_blend = float4::one;

	//Catmull-rom
	Spline path_top, path_bottom;
	std::vector<float3> spline, spline_top, spline_bottom;
	int points_in_curve = 5; // Begin with 5 points for a smooth Curve

private:
	unsigned int trail_vao, trail_vbo;

};

#endif
