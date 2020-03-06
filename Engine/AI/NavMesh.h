#ifndef _NAVMESH_H_
#define _NAVMESH_H_

#include "AI/SampleDebugDraw.h"
#include "AI/DebugDrawGL.h"
#include "recast/Recast/Recast.h"
#include "MathGeoLib.h"
#include <pcg_basic.h>
#include <vector>
#include "Helper/Timer.h"

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

enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water)
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump
	SAMPLE_POLYFLAGS_DISABLED = 0x10,	// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff		// All abilities
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
	void RenderNavMesh(ComponentCamera& camera);
	bool RenderNavMesh(DuDebugDraw* dd, const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags) const;
	void RenderTile(DuDebugDraw* dd, const dtNavMesh& mesh, const dtNavMeshQuery* query, const dtMeshTile* tile, unsigned char flags) const;

	inline SampleDebugDraw& GetDebugDraw() { return m_dd; }

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

public:
	uint64_t mesh_floor_uuid;

protected:
	unsigned char* m_triareas = nullptr;
	rcHeightfield* m_solid = nullptr;
	rcCompactHeightfield* m_chf = nullptr;
	rcContourSet* m_cset = nullptr;
	rcPolyMesh* m_pmesh = nullptr;
	rcConfig m_cfg;
	rcPolyMeshDetail* m_dmesh = nullptr;

	DrawMode m_drawMode;

	// Variables of NavMesh (modified by UI)
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
	Timer navmesh_timer;
	float time_to_build = 0.0f;

	friend PanelNavMesh;
};

#endif // _NAVMESH_H_