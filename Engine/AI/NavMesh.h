#ifndef _NAVMESH_H_
#define _NAVMESH_H_

#include <pcg_basic.h>
#include "recast/Recast/Recast.h"
#include "MathGeoLib.h"
#include <vector>
#include "recast/DebugUtils/DebugDraw.h"
#include "Component/ComponentCamera.h"

enum DrawMode
{
	DRAWMODE_NAVMESH,
	DRAWMODE_NAVMESH_TRANS,
	DRAWMODE_NAVMESH_BVTREE,
	DRAWMODE_NAVMESH_NODES,
	DRAWMODE_NAVMESH_INVIS,
	DRAWMODE_MESH,
	DRAWMODE_VOXELS,
	DRAWMODE_VOXELS_WALKABLE,
	DRAWMODE_COMPACT,
	DRAWMODE_COMPACT_DISTANCE,
	DRAWMODE_COMPACT_REGIONS,
	DRAWMODE_REGION_CONNECTIONS,
	DRAWMODE_RAW_CONTOURS,
	DRAWMODE_BOTH_CONTOURS,
	DRAWMODE_CONTOURS,
	DRAWMODE_POLYMESH,
	DRAWMODE_POLYMESH_DETAIL,
	MAX_DRAWMODE
};

enum SamplePartitionType
{
	SAMPLE_PARTITION_WATERSHED,
	SAMPLE_PARTITION_MONOTONE,
	SAMPLE_PARTITION_LAYERS,
};

/// These are just sample areas to use consistent values across the samples.
/// The use should specify these base on his needs.
enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};

enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};

class DebugDrawGL : public duDebugDraw
{

public:
	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);
	virtual void end();

	virtual void drawMesh(ComponentCamera& camera);

	unsigned int vbo = 0;
	unsigned int vao = 0;

	std::vector<float3> vertices;

};

class SampleDebugDraw : public DebugDrawGL
{
public:
	virtual unsigned int areaToCol(unsigned int area);
};


class dtNavMeshQuery;
class dtNavMesh;
class DetourNavMesh;
class dtMeshTile;
class PanelNavMesh;

class NavMesh
{
public:
	NavMesh();
	~NavMesh();
	bool CleanUp();

	bool Update();

	bool CreateNavMesh();
	bool RenderNavMesh(duDebugDraw* dd, const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags) const;
	void RenderTile(duDebugDraw* dd, const dtNavMesh& mesh, const dtNavMeshQuery* query, const dtMeshTile* tile, unsigned char flags) const;
	void RenderNavMesh(ComponentCamera& camera);

	SampleDebugDraw& getDebugDraw() { return m_dd; }

private:
	void GetVerticesScene();
	void GetIndicesScene();
	void GetNormalsScene();

	float DistancePtLine2d(const float* pt, const float* p, const float* q) const
	{
		float pqx = q[0] - p[0];
		float pqz = q[2] - p[2];
		float dx = pt[0] - p[0];
		float dz = pt[2] - p[2];
		float d = pqx * pqx + pqz * pqz;
		float t = pqx * dx + pqz * dz;
		if (d != 0) t /= d;
		dx = p[0] + t * pqx - pt[0];
		dz = p[2] + t * pqz - pt[2];
		return dx * dx + dz * dz;
	}

protected:
	unsigned char* m_triareas = nullptr;
	rcHeightfield* m_solid = nullptr;
	rcCompactHeightfield* m_chf = nullptr;
	rcContourSet* m_cset = nullptr;
	rcPolyMesh* m_pmesh = nullptr;
	rcConfig m_cfg;
	rcPolyMeshDetail* m_dmesh = nullptr;

	
	DrawMode m_drawMode;

	//Variables of NavMesh (modified by UI)
	float cell_width = 0.30f;
	float cell_height = 0.20f;
	float walkable_slope_angle = 50.0f;
	float agent_height = 2.0f;
	float agent_max_climb = 0.09f;
	float agent_radius = 0.6f;
	float edge_max_len = 12.0f;
	float edge_max_error = 1.3f;
	int region_min_size = 8;
	int region_merge_size = 20;
	float verts_per_poly = 6;
	float detail_sample_distance = 6;
	float detail_sample_max_error = 1;

	const float texScale = 1.f / ((cell_width * cell_height) / 10.f);

public:
	uint64_t mesh_floor_uuid;


private:
	rcContext* m_ctx;

	bool keep_inter_results = true;
	bool filter_low_hanging_obstacles = true;
	bool filter_ledge_spans = true;
	bool filter_walkable_low_height_spans = true;


	SamplePartitionType partition_type = SAMPLE_PARTITION_WATERSHED;

	//Data
	std::vector<float> verts_vec;
	std::vector<int> tris_vec;
	std::vector<float> normals_vec;
	AABB global_AABB;
	bool is_mesh_computed = false;

	dtNavMeshQuery* nav_query = nullptr;
	dtNavMesh* nav_mesh = nullptr;
	unsigned char nav_mesh_draw_flags;


	///TEST
	SampleDebugDraw m_dd;
	std::vector<float> vertices;


	friend PanelNavMesh;
};

#endif // _NAVMESH_H_