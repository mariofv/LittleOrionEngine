#ifndef _NAVMESH_H_
#define _NAVMESH_H_

#include "AI/SampleDebugDraw.h"
#include "AI/DebugDrawGL.h"
#include "Helper/Timer.h"

#include <MathGeoLib.h>
#include <pcg_basic.h>
#include <recast/Detour/DetourNavMesh.h>
#include <recast/Recast/Recast.h>
#include <vector>

class DetourNavMesh;
class dtNavMeshQuery;
class dtNavMesh;
class PanelNavMesh;

struct dtMeshTile;

enum PathMode
{
	FOLLOW_PATH,
	STRAIGHT_PATH
};

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

class NavMesh
{
public:
	NavMesh();
	~NavMesh();

	bool CleanUp();
	bool Update();

	bool CreateNavMesh();
	void RenderNavMesh(ComponentCamera& camera);
	void InitAABB();

	bool FindPath(float3& start, float3& end, std::vector<float3>& path, PathMode path_mode);
	bool IsPointWalkable(float3 & target_position);
	bool FindNextPolyByDirection(float3& position, float3& next_position);

	void SaveNavMesh(unsigned char* nav_data, unsigned int nav_data_size) const;
	void LoadNavMesh();

	inline SampleDebugDraw& GetDebugDraw() { return m_dd; }

private:
	void GetVerticesScene();
	void GetIndicesScene();
	void GetNormalsScene();

	bool GetSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
		const float minTargetDist,
		const dtPolyRef* path, const int pathSize,
		float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
		float* outPoints = 0, int* outPointCount = 0);

	inline bool InRange(const float* v1, const float* v2, const float r, const float h);
	int FixUpShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* nav_query);
	int FixUpCorridor(dtPolyRef* path, const int npath, const int max_path,
		const dtPolyRef* visited, const int nvisited);

	float DistancePtLine2d(const float* pt, const float* p, const float* q) const;
	void drawMeshTile(SampleDebugDraw* dd, const dtNavMesh& mesh, const dtNavMeshQuery* query,
		const dtMeshTile* tile, unsigned char flags);
	void duDebugDrawNavMesh(SampleDebugDraw* dd, const dtNavMesh& mesh, unsigned char flags);
	void duDebugDrawNavMeshWithClosedList(SampleDebugDraw* dd, const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags);

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
	float cell_width = 0.20f;
	float cell_height = 1.0f;
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

	//If true there are memory leaks but it should be only for debugging
	bool keep_inter_results = false;
	bool filter_low_hanging_obstacles = true;
	bool filter_ledge_spans = true;
	bool filter_walkable_low_height_spans = true;

	SamplePartitionType partition_type = SAMPLE_PARTITION_WATERSHED;

	//Data
	std::vector<float> verts_vec;
	std::vector<int> tris_vec;
	std::vector<float> normals_vec;
	std::vector<bool> unwalkable_verts;

	char* navmesh_read_data = nullptr;

	int ntris = 0;


	AABB global_AABB;
	bool is_mesh_computed = false;

	dtNavMeshQuery* nav_query = nullptr;
	dtNavMesh* nav_mesh = nullptr;
	unsigned char nav_mesh_draw_flags;

	//PathMode path_mode = PathMode::FOLLOW_PATH;


	static const int MAX_POLYS = 256;
	static const int MAX_SMOOTH = 2048;

	///TEST
	SampleDebugDraw m_dd;
	Timer navmesh_timer;
	float time_to_build = 0.0f;

	friend PanelNavMesh;
};

#endif // _NAVMESH_H_