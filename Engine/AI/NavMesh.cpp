#include "NavMesh.h"

#include "Component/ComponentMeshRenderer.h"
#include "Component/ComponentCamera.h"

#include "EditorUI/DebugDraw.h"

#include "Filesystem/PathAtlas.h"

#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"

#include <math.h>
#include <recast/Detour/DetourCommon.h>
#include <recast/DebugUtils/DetourDebugDraw.h>
#include <recast/Detour/DetourNavMeshQuery.h>
#include <recast/Detour/DetourNavMeshBuilder.h>


///https://masagroup.github.io/recastdetour/group__recast.html nais documentation

//TODO: Try to use the TemporalObstacle aproach to handle the multimesh navmesh
// Using dtTileCache for multiple tiles.

NavMesh::NavMesh()
{
	m_ctx = new rcContext();
	nav_query = dtAllocNavMeshQuery();
	navmesh_timer = Timer();
}

NavMesh::~NavMesh()
{
	CleanUp();
}

bool NavMesh::CleanUp()
{
	m_dd.CleanUp();
	is_mesh_computed = false;

	return true;
}

bool NavMesh::Update()
{
	return true;
}

bool NavMesh::CreateNavMesh()
{
	CleanUp();

	navmesh_timer.Start();

	GetVerticesScene();
	GetIndicesScene();
	GetNormalsScene();
	
	InitAABB();


	const float* bmin = &global_AABB.minPoint[0];
	const float* bmax = &global_AABB.maxPoint[0];
	const float* verts = &verts_vec[0];
	const int nverts = verts_vec.size();
	const int* tris = &tris_vec[0];
	ntris = tris_vec.size() / 3;
	const float* normals = &normals_vec[0];

	
	//
// Step 1. Initialize build config.
//

// Init build configuration from GUI
	m_cfg = rcConfig();
	memset(&m_cfg, 0, sizeof(m_cfg));

	m_cfg.cs = cell_width;
	m_cfg.ch = cell_height;
	m_cfg.walkableSlopeAngle = walkable_slope_angle;
	//walkableHeight	Minimum floor to 'ceiling' height that will still allow the floor area to be considered walkable. [Limit: >= 3]
	m_cfg.walkableHeight = (int)ceilf(agent_height / m_cfg.ch);
	m_cfg.walkableClimb = (int)floorf(agent_max_climb / m_cfg.ch);
	//walkableRadius	The radius of erosion. [Limits: 0 < value < 255] [Units: vx] 
	m_cfg.walkableRadius = (int)ceilf(agent_radius / m_cfg.cs);
	m_cfg.maxEdgeLen = (int)(edge_max_len / cell_width);
	m_cfg.maxSimplificationError = edge_max_error;
	//minRegionArea	The minimum number of cells allowed to form isolated island areas.[Limit:>= 0][Units:vx].
	m_cfg.minRegionArea = (int)rcSqr(region_min_size);		// Note: area = size*size
	//mergeRegionArea	Any regions with a span count smaller than this value will, if possible, be merged with larger regions. [Limit: >=0] [Units: vx] 
	m_cfg.mergeRegionArea = (int)rcSqr(region_merge_size);	// Note: area = size*size
	m_cfg.maxVertsPerPoly = (int)verts_per_poly;
	m_cfg.detailSampleDist = detail_sample_distance < 0.9f ? 0 : cell_width * detail_sample_distance;
	m_cfg.detailSampleMaxError = cell_height * detail_sample_max_error;

	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcVcopy(m_cfg.bmin, bmin);
	rcVcopy(m_cfg.bmax, bmax);
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

	APP_LOG_INFO("Building navigation:");
	APP_LOG_INFO("- %d x %d cells", m_cfg.width, m_cfg.height);
	APP_LOG_INFO(" - %.1fK verts, %.1fK tris", nverts / 1000.0f, ntris / 1000.0f);

	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.

	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		APP_LOG_ERROR("buildNavigation: Out of memory 'solid'.");
		return false;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		APP_LOG_ERROR("buildNavigation: Could not create solid heightfield.");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[ntris];
	if (!m_triareas)
	{
		APP_LOG_ERROR("buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the area type for each of the meshes and rasterize them.
	memset(m_triareas, 0, ntris * sizeof(unsigned char));
	rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas, unwalkable_verts);
	if (!rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb))
	{
		APP_LOG_ERROR("buildNavigation: Could not rasterize triangles.");
		return false;
	}

	if (!keep_inter_results)
	{
		delete[] m_triareas;
		m_triareas = 0;
	}

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	if (filter_low_hanging_obstacles)
		rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
	if (filter_ledge_spans)
		rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	if (filter_walkable_low_height_spans)
		rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);

	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf)
	{
		APP_LOG_ERROR("buildNavigation: Out of memory 'chf'.");
		return false;
	}
	if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		APP_LOG_ERROR("buildNavigation: Could not build compact data.");
		return false;
	}

	if (!keep_inter_results)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
	{
		APP_LOG_ERROR("buildNavigation: Could not erode.");
		return false;
	}

	// (Optional) Mark areas.
	//const ConvexVolume* vols = m_geom->getConvexVolumes();
	//for (int i = 0; i < m_geom->getConvexVolumeCount(); ++i)
		//rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);


	// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
	// There are 3 martitioning methods, each with some pros and cons:
	// 1) Watershed partitioning
	//   - the classic Recast partitioning
	//   - creates the nicest tessellation
	//   - usually slowest
	//   - partitions the heightfield into nice regions without holes or overlaps
	//   - the are some corner cases where this method creates produces holes and overlaps
	//      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
	//      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
	//   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
	// 2) Monotone partioning
	//   - fastest
	//   - partitions the heightfield into regions without holes and overlaps (guaranteed)
	//   - creates long thin polygons, which sometimes causes paths with detours
	//   * use this if you want fast navmesh generation
	// 3) Layer partitoining
	//   - quite fast
	//   - partitions the heighfield into non-overlapping regions
	//   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
	//   - produces better triangles than monotone partitioning
	//   - does not have the corner cases of watershed partitioning
	//   - can be slow and create a bit ugly tessellation (still better than monotone)
	//     if you have large open areas with small obstacles (not a problem if you use tiles)
	//   * good choice to use for tiled navmesh with medium and small sized tiles

	if (partition_type == SAMPLE_PARTITION_WATERSHED)
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if (!rcBuildDistanceField(m_ctx, *m_chf))
		{
			APP_LOG_ERROR("buildNavigation: Could not build distance field.");
			return false;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			APP_LOG_ERROR("buildNavigation: Could not build watershed regions.");
			return false;
		}
	}
	else if (partition_type == SAMPLE_PARTITION_MONOTONE)
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distancefield.
		if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
			APP_LOG_ERROR("buildNavigation: Could not build monotone regions.");
			return false;
		}
	}
	else // SAMPLE_PARTITION_LAYERS
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildLayerRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea))
		{
			APP_LOG_ERROR("buildNavigation: Could not build layer regions.");
			return false;
		}
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset)
	{
		APP_LOG_ERROR("buildNavigation: Out of memory 'cset'.");
		return false;
	}
	if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
	{
		APP_LOG_ERROR("buildNavigation: Could not create contours.");
		return false;
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		APP_LOG_ERROR("buildNavigation: Out of memory 'pmesh'.");
		return false;
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		APP_LOG_ERROR("buildNavigation: Could not triangulate contours.");
		return false;
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		APP_LOG_ERROR("buildNavigation: Out of memory 'pmdtl'.");
		return false;
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
	{
		APP_LOG_ERROR("buildNavigation: Could not build detail mesh.");
		return false;
	}

	if (!keep_inter_results)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}
	
	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//
	
	// The GUI may allow more max points per polygon than Detour can handle.
	// Only build the detour navmesh if we do not exceed the limit.
	if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* nav_data = 0;
		int nav_data_size = 0;

		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
				m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

			if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}


		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;
		//params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
		//params.offMeshConRad = m_geom->getOffMeshConnectionRads();
		//params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
		//params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
		//params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
		//params.offMeshConUserID = m_geom->getOffMeshConnectionId();
		params.offMeshConCount = 0;
		params.walkableHeight = agent_height;
		params.walkableRadius = agent_radius;
		params.walkableClimb = agent_max_climb;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;
		params.buildBvTree = true;

		if (!dtCreateNavMeshData(&params, &nav_data, &nav_data_size))
		{
			APP_LOG_ERROR("Could not build Detour navmesh.");
			return false;
		}


		//Save nav_data on a file
		SaveNavMesh(nav_data, nav_data_size);

		nav_mesh = dtAllocNavMesh();
		if (!nav_mesh)
		{
			dtFree(nav_data);
			APP_LOG_ERROR("Could not create Detour navmesh");
			return false;
		}

		dtStatus status;

		status = nav_mesh->init(nav_data, nav_data_size, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(nav_data);
			APP_LOG_ERROR("Could not init Detour navmesh");
			return false;
		}

		status = nav_query->init(nav_mesh, 2048);
		if (dtStatusFailed(status))
		{
			APP_LOG_ERROR("Could not init Detour navmesh query");
			return false;
		}
	}

	
	time_to_build = navmesh_timer.Stop();

	is_mesh_computed = true;
	duDebugDrawNavMeshWithClosedList(&m_dd, *nav_mesh, *nav_query, nav_mesh_draw_flags);
	m_dd.GenerateBuffers();

	return true;
}

void NavMesh::RenderNavMesh(ComponentCamera& camera)
{
	if (is_mesh_computed)
	{
		m_dd.DrawMesh(camera);
	}
}

void NavMesh::InitAABB()
{
	global_AABB.SetNegativeInfinity();

	for (const auto& mesh : App->renderer->meshes)
	{
		float minX = std::fmin(mesh->owner->aabb.bounding_box.minPoint.x, global_AABB.minPoint.x);
		float minY = std::fmin(mesh->owner->aabb.bounding_box.minPoint.y, global_AABB.minPoint.y);
		float minZ = std::fmin(mesh->owner->aabb.bounding_box.minPoint.z, global_AABB.minPoint.z);


		float maxX = std::fmax(mesh->owner->aabb.bounding_box.maxPoint.x, global_AABB.maxPoint.x);
		float maxY = std::fmax(mesh->owner->aabb.bounding_box.maxPoint.y, global_AABB.maxPoint.y);
		float maxZ = std::fmax(mesh->owner->aabb.bounding_box.maxPoint.z, global_AABB.maxPoint.z);


		global_AABB.maxPoint = float3(maxX, maxY, maxZ);
		global_AABB.minPoint = float3(minX, minY, minZ);

	}
}

bool NavMesh::FindPath(float3& start, float3& end, std::vector<float3>& path, PathMode path_mode)
{

	path.clear();

	if(!nav_query)
	{
		APP_LOG_ERROR("Cannot find path if dtNavMeshQuery* nav_query is nullptr");
		return false;
	}

	dtPolyRef start_ref, end_ref;
	dtQueryFilter filter;
	filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	filter.setExcludeFlags(0);

	float poly_pick_ext[3] = { 2.0F, 4.0F, 2.0F};

	nav_query->findNearestPoly((float*)&start, poly_pick_ext, &filter, &start_ref, 0);
	nav_query->findNearestPoly((float*)&end, poly_pick_ext, &filter, &end_ref, 0);

	

	if(!start_ref)
	{
		APP_LOG_INFO("Couldn't find any polygon on start position.");
		return false;
	}

	if (!end_ref)
	{
		APP_LOG_INFO("Couldn't find any polygon on end position.");
		return false;
	}

	dtPolyRef path_ref[MAX_POLYS];
	int path_count = 0;

	dtStatus status = nav_query->findPath(start_ref, end_ref, (float*)&start, (float*)&end, &filter, path_ref, &path_count, MAX_POLYS);

	if(dtStatusFailed(status))
	{
		APP_LOG_INFO("Couldn't find a path from start to end.");
		return false;
	
	}

	if(path_mode == PathMode::FOLLOW_PATH)
	{

		if(path_count)
		{
			// Iterate over the path to find smooth path on the detail mesh surface.
			dtPolyRef polys[MAX_POLYS];
			memcpy(polys, path_ref, sizeof(dtPolyRef)*path_count);
			int npolys = path_count;

			float iter_pos[3]; 
			float target_pos[3];
			nav_query->closestPointOnPoly(start_ref, (float*)&start, iter_pos, 0);
			nav_query->closestPointOnPoly(polys[npolys - 1], (float*)&end, target_pos, 0);

			static const float STEP_SIZE = 0.5F;
			static const float SLOP = 0.01F;

			int nsmooth_path = 0;

			float smooth_path[MAX_POLYS * 3];

			dtVcopy(&smooth_path[nsmooth_path * 3], iter_pos);
			nsmooth_path++;

			// Move towards target a small advancement at a time until target reached or
			// when ran out of memory to store the path.
			while (npolys && nsmooth_path < MAX_SMOOTH)
			{
				// Find location to steer towards.
				float steer_pos[3];
				unsigned char steer_pos_flag;
				dtPolyRef steer_pos_ref;

				if (!GetSteerTarget(nav_query, iter_pos, target_pos, SLOP,
					polys, npolys, steer_pos, steer_pos_flag, steer_pos_ref))
					break;

				bool end_of_path = (steer_pos_flag & DT_STRAIGHTPATH_END) ? true : false;
				bool off_mesh_connection = (steer_pos_flag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;

				// Find movement delta.
				float delta[3];
				float len;
				dtVsub(delta, steer_pos, iter_pos);
				// sqrt of vector by itself to get the magnitud of the vector delta
				len = dtMathSqrtf(dtVdot(delta, delta));

				// If the steer target is end of path or off-mesh link, do not move past the location.
				if ((end_of_path || off_mesh_connection) && len < STEP_SIZE)
					len = 1;
				else
					len = STEP_SIZE / len;
				float move_tgt[3];
				// Add delta to iter_pos and scale by len to get the target vector
				dtVmad(move_tgt, iter_pos, delta, len);

				// Move
				float result[3];
				dtPolyRef visited[16];
				int nvisited = 0;
				nav_query->moveAlongSurface(polys[0], iter_pos, move_tgt, &filter,
					result, visited, &nvisited, 16);

				npolys = FixUpCorridor(polys, npolys, MAX_POLYS, visited, nvisited);
				npolys = FixUpShortcuts(polys, npolys, nav_query);

				float h = 0;
				nav_query->getPolyHeight(polys[0], result, &h);
				result[1] = h;
				dtVcopy(iter_pos, result);

				// Handle end of path and off-mesh links when close enough.
				if (end_of_path && InRange(iter_pos, steer_pos, SLOP, 1.0F))
				{
					// Reached end of path.
					dtVcopy(iter_pos, target_pos);
					if (nsmooth_path < MAX_SMOOTH)
					{
						dtVcopy(&smooth_path[nsmooth_path * 3], iter_pos);
						nsmooth_path++;
					}
					break;
				}
				else if (off_mesh_connection && InRange(iter_pos, steer_pos, SLOP, 1.0F))
				{
					// Reached off-mesh connection.
					float startPos[3];
					float endPos[3];

					// Advance the path up to and over the off-mesh connection.
					dtPolyRef prevRef = 0, polyRef = polys[0];
					int npos = 0;
					while (npos < npolys && polyRef != steer_pos_ref)
					{
						prevRef = polyRef;
						polyRef = polys[npos];
						npos++;
					}
					for (int i = npos; i < npolys; ++i)
						polys[i - npos] = polys[i];
					npolys -= npos;

					// Handle the connection.
					dtStatus status = nav_mesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
					if (dtStatusSucceed(status))
					{
						if (nsmooth_path < MAX_SMOOTH)
						{
							dtVcopy(&smooth_path[nsmooth_path * 3], startPos);
							nsmooth_path++;
							// Hack to make the dotted path not visible during off-mesh connection.
							if (nsmooth_path & 1)
							{
								dtVcopy(&smooth_path[nsmooth_path * 3], startPos);
								nsmooth_path++;
							}
						}
						// Move position at the other side of the off-mesh link.
						dtVcopy(iter_pos, endPos);
						float eh = 0.0F;
						nav_query->getPolyHeight(polys[0], iter_pos, &eh);
						iter_pos[1] = eh;
					}
				}

				// Store results.
				if (nsmooth_path < MAX_SMOOTH)
				{
					dtVcopy(&smooth_path[nsmooth_path * 3], iter_pos);
					nsmooth_path++;
				}

			}

			for(int i = 0; i < nsmooth_path; ++i)
			{
				path.push_back(float3(smooth_path[i * 3], smooth_path[i * 3 + 1], smooth_path[i * 3 + 2]));
			}

		}
			/*
		float3 current_position = start;

		for (unsigned int i = 0; i < path_count; ++i)
		{
			if (nav_query->isValidPolyRef(path_ref[i], &filter))
			{
				float3 path_position;
				nav_query->closestPointOnPoly(path_ref[i], (float*)&current_position, (float*)&path_position, 0);

				path.push_back(path_position);
				current_position = path_position;
			}
		}
		*/
	}
	else if(path_mode == PathMode::STRAIGHT_PATH)
	{
		if (path_count)
		{

			float straight_path[MAX_POLYS * 3];
			int number_of_points = 0;
			dtStatus straight_status = nav_query->findStraightPath((float*)&start, (float*)&end,
				path_ref, path_count, straight_path, NULL, NULL, &number_of_points, MAX_POLYS);

			if (dtStatusFailed(straight_status))
			{
				APP_LOG_INFO("Couldn't find a straight path from start to end.");
				return false;
			}

			for (int i = 0; i < number_of_points; ++i)
			{
				path.push_back(float3(straight_path[i * 3], straight_path[i * 3 + 1], straight_path[i * 3 + 2]));
			}
		}
	}


	return true;
}

bool NavMesh::IsPointWalkable(float3 & target_position)
{
	dtPolyRef target_ref;
	dtQueryFilter filter;
	filter.setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	filter.setExcludeFlags(0);

	float3 diff = math::float3(0.1f, 2.f, 0.1f);
	float poly_pick_ext[3] = { diff.x, diff.y, diff.z };

	nav_query->findNearestPoly((float*)&target_position, poly_pick_ext, &filter, &target_ref, diff.ptr());

	if (target_ref == 0)
		return false;

	return Distance(diff, target_position) < 0.45f;
}

bool NavMesh::FindNextPolyByDirection(float3& position, float3& next_position)
{
	float3 aux_position = position;
	dtPolyRef target_ref;
	dtQueryFilter filter;
	float3 diff = math::float3(0.1f, 2.f, 0.1f);
	float poly_pick_ext[3] = { diff.x, diff.y, diff.z };

	nav_query->findNearestPoly((float*)&aux_position, poly_pick_ext, &filter, &target_ref, diff.ptr());
	//PolyNotFound
	if(target_ref == 0)
	{
		return false;
	}
	
	float pos[3];
	nav_query->closestPointOnPoly(target_ref, (float*)&aux_position, pos, 0);

	next_position.x = pos[0];
	next_position.y = pos[1];
	next_position.z = pos[2];

	return true;
}



void NavMesh::SaveNavMesh(unsigned char* nav_data, unsigned int nav_data_size) const
{
	Path* navmesh_path = App->filesystem->GetPath(RESOURCES_NAVMESH_PATH);
	std::string navmesh_filename("survival_scene_navmesh.bin");

	unsigned char* copy_nav_data = new unsigned char[nav_data_size];
	memcpy(copy_nav_data, nav_data, nav_data_size);

	FileData navmesh_data{ reinterpret_cast<char*>(copy_nav_data), nav_data_size };

	navmesh_path->Save(navmesh_filename.c_str(), navmesh_data);
}

void NavMesh::LoadNavMesh()
{
	std::string navmesh_file_path_string(RESOURCES_NAVMESH_PATH);
	navmesh_file_path_string.append("/survival_scene_navmesh.bin");

	if (!App->filesystem->Exists(navmesh_file_path_string))
	{
		APP_LOG_ERROR("Cannot load navmesh.");
		return;
	}

	Path* navmesh_file_path = App->filesystem->GetPath(navmesh_file_path_string);
	FileData navmesh_data = navmesh_file_path->GetFile()->Load();

	size_t readed_bytes = navmesh_data.size;
	navmesh_read_data = (char*)navmesh_data.buffer;

	nav_mesh = dtAllocNavMesh();
	if (!nav_mesh)
	{
		dtFree(navmesh_read_data);
		APP_LOG_ERROR("Could not create Detour navmesh");
		return;
	}

	dtStatus status;

	status = nav_mesh->init(reinterpret_cast<unsigned char*>(navmesh_read_data), (unsigned int)readed_bytes, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navmesh_read_data);
		APP_LOG_ERROR("Could not init Detour navmesh");
		return;
	}

	status = nav_query->init(nav_mesh, 2048);
	if (dtStatusFailed(status))
	{
		APP_LOG_ERROR("Could not init Detour navmesh query");
		return;
	}


	is_mesh_computed = true;
	duDebugDrawNavMeshWithClosedList(&m_dd, *nav_mesh, *nav_query, nav_mesh_draw_flags);
	m_dd.GenerateBuffers();

}

void NavMesh::GetVerticesScene()
{
	//Clear vertex vector
	verts_vec.clear();
	unwalkable_verts.clear();

	for (const auto& mesh : App->renderer->meshes)
	{
		for (size_t i = 0; i < mesh->mesh_to_render.get()->vertices.size(); ++i)
		{
			float4 vertss(mesh->mesh_to_render.get()->vertices[i].position, 1.0f);
			vertss = mesh->owner->transform.GetGlobalModelMatrix() * vertss;

			verts_vec.push_back(vertss.x);
			verts_vec.push_back(vertss.y);
			verts_vec.push_back(vertss.z);

			//TODO: walkable optimization
		}
	}
}

void NavMesh::GetIndicesScene()
{
	//Clear index vector
	tris_vec.clear();

	if (!App->renderer->meshes.size())
		return;

	std::vector<int>max_vert_mesh(App->renderer->meshes.size() + 1, 0);
	for(size_t i = 0; i < App->renderer->meshes.size(); ++i)
	{
		ntris += App->renderer->meshes[i]->mesh_to_render.get()->indices.size() / 3;
		max_vert_mesh[i + 1] = App->renderer->meshes[i]->mesh_to_render.get()->vertices.size();
	}

	int vert_overload = 0;
	for(size_t j = 0; j < App->renderer->meshes.size(); ++j)
	{
		vert_overload += max_vert_mesh[j];
		for(size_t i = 0; i < App->renderer->meshes[j]->mesh_to_render.get()->indices.size(); i+= 3)
		{
			tris_vec.push_back(App->renderer->meshes[j]->mesh_to_render.get()->indices[i] + vert_overload);
			tris_vec.push_back(App->renderer->meshes[j]->mesh_to_render.get()->indices[i + 1] + vert_overload);
			tris_vec.push_back(App->renderer->meshes[j]->mesh_to_render.get()->indices[i + 2] + vert_overload);

		}
	}

}

void NavMesh::GetNormalsScene()
{
	//Clear normals vector
	normals_vec.clear();

	for (const const auto&  mesh : App->renderer->meshes)
	{
		for (size_t i = 0; i < mesh->mesh_to_render.get()->vertices.size(); ++i)
		{
			normals_vec.push_back(mesh->mesh_to_render.get()->vertices[i].normals.x);
			normals_vec.push_back(mesh->mesh_to_render.get()->vertices[i].normals.y);
			normals_vec.push_back(mesh->mesh_to_render.get()->vertices[i].normals.z);
		}
	}
}

bool NavMesh::GetSteerTarget(dtNavMeshQuery * navQuery, const float * startPos, 
	const float * endPos, const float minTargetDist, const dtPolyRef * path, 
	const int pathSize, float * steerPos, unsigned char & steerPosFlag, dtPolyRef & steerPosRef, 
	float * outPoints, int * outPointCount)
{
	// Find steer target.
	static const int MAX_STEER_POINTS = 3;
	float steer_path[MAX_STEER_POINTS * 3];
	unsigned char steer_path_flags[MAX_STEER_POINTS];
	dtPolyRef steer_path_polys[MAX_STEER_POINTS];
	int nsteer_path = 0;
	nav_query->findStraightPath(startPos, endPos, path, pathSize,
		steer_path, steer_path_flags, steer_path_polys, &nsteer_path, MAX_STEER_POINTS);
	if (!nsteer_path)
		return false;

	if (outPoints && outPointCount)
	{
		*outPointCount = nsteer_path;
		for (int i = 0; i < nsteer_path; ++i)
			dtVcopy(&outPoints[i * 3], &steer_path[i * 3]);
	}


	// Find vertex far enough to steer to.
	int ns = 0;
	while (ns < nsteer_path)
	{
		// Stop at Off-Mesh link or when point is further than slop away.
		if ((steer_path_flags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
			!InRange(&steer_path[ns * 3], startPos, minTargetDist, 1000.0f))
			break;
		ns++;
	}
	// Failed to find good point to steer to.
	if (ns >= nsteer_path)
		return false;

	dtVcopy(steerPos, &steer_path[ns * 3]);
	steerPos[1] = startPos[1];
	steerPosFlag = steer_path_flags[ns];
	steerPosRef = steer_path_polys[ns];

	return true;
}

inline bool NavMesh::InRange(const float * v1, const float * v2, const float r, const float h)
{
	const float dx = v2[0] - v1[0];
	const float dy = v2[1] - v1[1];
	const float dz = v2[2] - v1[2];
	return (dx*dx + dz * dz) < r*r && fabsf(dy) < h;
}

// This function checks if the path has a small U-turn, that is,
// a polygon further in the path is adjacent to the first polygon
// in the path. If that happens, a shortcut is taken.
// This can happen if the target (T) location is at tile boundary,
// and we're (S) approaching it parallel to the tile edge.
// The choice at the vertex can be arbitrary, 
//  +---+---+
//  |:::|:::|
//  +-S-+-T-+
//  |:::|   | <-- the step can end up in here, resulting U-turn path.
//  +---+---+

int NavMesh::FixUpShortcuts(dtPolyRef * path, int npath, dtNavMeshQuery * nav_query)
{
	if (npath < 3)
		return npath;

	// Get connected polygons
	static const int maxNeis = 16;
	dtPolyRef neis[maxNeis];
	int nneis = 0;

	const dtMeshTile* tile = 0;
	const dtPoly* poly = 0;
	if (dtStatusFailed(nav_query->getAttachedNavMesh()->getTileAndPolyByRef(path[0], &tile, &poly)))
		return npath;

	for (unsigned int k = poly->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
	{
		const dtLink* link = &tile->links[k];
		if (link->ref != 0)
		{
			if (nneis < maxNeis)
				neis[nneis++] = link->ref;
		}
	}

	// If any of the neighbour polygons is within the next few polygons
	// in the path, short cut to that polygon directly.
	static const int maxLookAhead = 6;
	int cut = 0;
	for (int i = dtMin(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--) {
		for (int j = 0; j < nneis; j++)
		{
			if (path[i] == neis[j]) {
				cut = i;
				break;
			}
		}
	}
	if (cut > 1)
	{
		int offset = cut - 1;
		npath -= offset;
		for (int i = 1; i < npath; i++)
			path[i] = path[i + offset];
	}

	return npath;
}

int NavMesh::FixUpCorridor(dtPolyRef * path, const int npath, const int max_path, const dtPolyRef * visited, const int nvisited)
{
	int furthestPath = -1;
	int furthestVisited = -1;

	// Find furthest common polygon.
	for (int i = npath - 1; i >= 0; --i)
	{
		bool found = false;
		for (int j = nvisited - 1; j >= 0; --j)
		{
			if (path[i] == visited[j])
			{
				furthestPath = i;
				furthestVisited = j;
				found = true;
			}
		}
		if (found)
			break;
	}

	// If no intersection found just return current path. 
	if (furthestPath == -1 || furthestVisited == -1)
		return npath;

	// Concatenate paths.	

	// Adjust beginning of the buffer to include the visited.
	const int req = nvisited - furthestVisited;
	const int orig = rcMin(furthestPath + 1, npath);
	int size = rcMax(0, npath - orig);
	if (req + size > max_path)
		size = max_path - req;
	if (size)
		memmove(path + req, path + orig, size * sizeof(dtPolyRef));

	// Store visited
	for (int i = 0; i < req; ++i)
		path[i] = visited[(nvisited - 1) - i];

	return req + size;
}

float NavMesh::DistancePtLine2d(const float * pt, const float * p, const float * q) const
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

void NavMesh::drawMeshTile(SampleDebugDraw* dd, const dtNavMesh& mesh, const dtNavMeshQuery* query,
	const dtMeshTile* tile, unsigned char flags)
{
	dtPolyRef base = mesh.getPolyRefBase(tile);

	int tileNum = mesh.decodePolyIdTile(base);
	const unsigned int tileColor = DuIntToCol(tileNum, 128);


	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];
		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
			continue;

		const dtPolyDetail* pd = &tile->detailMeshes[i];

		unsigned int col;
		if (query && query->isInClosedList(base | (dtPolyRef)i))
			col = DuRGBA(255, 196, 0, 64);
		else
		{
			if (flags & DU_DRAWNAVMESH_COLOR_TILES)
				col = tileColor;
			else
				col = DuTransCol(dd->AreaToCol(p->getArea()), 64);
		}

		for (int j = 0; j < pd->triCount; ++j)
		{
			const unsigned char* t = &tile->detailTris[(pd->triBase + j) * 4];
			for (int k = 0; k < 3; ++k)
			{
				if (t[k] < p->vertCount)
					dd->Vertex(&tile->verts[p->verts[t[k]] * 3], col);
				else
					dd->Vertex(&tile->detailVerts[(pd->vertBase + t[k] - p->vertCount) * 3], col);
			}
		}
	}
	/*
	// Draw inter poly boundaries
	drawPolyBoundaries(dd, tile, duRGBA(0,48,64,32), 1.5f, true);

	// Draw outer poly boundaries
	drawPolyBoundaries(dd, tile, duRGBA(0,48,64,220), 2.5f, false);

	if (flags & DU_DRAWNAVMESH_OFFMESHCONS)
	{
		dd->begin(DU_DRAW_LINES, 2.0f);
		for (int i = 0; i < tile->header->polyCount; ++i)
		{
			const dtPoly* p = &tile->polys[i];
			if (p->getType() != DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip regular polys.
				continue;

			unsigned int col, col2;
			if (query && query->isInClosedList(base | (dtPolyRef)i))
				col = duRGBA(255,196,0,220);
			else
				col = duDarkenCol(duTransCol(dd->areaToCol(p->getArea()), 220));

			const dtOffMeshConnection* con = &tile->offMeshCons[i - tile->header->offMeshBase];
			const float* va = &tile->verts[p->verts[0]*3];
			const float* vb = &tile->verts[p->verts[1]*3];

			// Check to see if start and end end-points have links.
			bool startSet = false;
			bool endSet = false;
			for (unsigned int k = p->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
			{
				if (tile->links[k].edge == 0)
					startSet = true;
				if (tile->links[k].edge == 1)
					endSet = true;
			}

			// End points and their on-mesh locations.
			dd->vertex(va[0],va[1],va[2], col);
			dd->vertex(con->pos[0],con->pos[1],con->pos[2], col);
			col2 = startSet ? col : duRGBA(220,32,16,196);
			duAppendCircle(dd, con->pos[0],con->pos[1]+0.1f,con->pos[2], con->rad, col2);

			dd->vertex(vb[0],vb[1],vb[2], col);
			dd->vertex(con->pos[3],con->pos[4],con->pos[5], col);
			col2 = endSet ? col : duRGBA(220,32,16,196);
			duAppendCircle(dd, con->pos[3],con->pos[4]+0.1f,con->pos[5], con->rad, col2);

			// End point vertices.
			dd->vertex(con->pos[0],con->pos[1],con->pos[2], duRGBA(0,48,64,196));
			dd->vertex(con->pos[0],con->pos[1]+0.2f,con->pos[2], duRGBA(0,48,64,196));

			dd->vertex(con->pos[3],con->pos[4],con->pos[5], duRGBA(0,48,64,196));
			dd->vertex(con->pos[3],con->pos[4]+0.2f,con->pos[5], duRGBA(0,48,64,196));

			// Connection arc.
			duAppendArc(dd, con->pos[0],con->pos[1],con->pos[2], con->pos[3],con->pos[4],con->pos[5], 0.25f,
						(con->flags & 1) ? 0.6f : 0, 0.6f, col);
		}
		dd->end();
	}

	const unsigned int vcol = duRGBA(0,0,0,196);
	dd->begin(DU_DRAW_POINTS, 3.0f);
	for (int i = 0; i < tile->header->vertCount; ++i)
	{
		const float* v = &tile->verts[i*3];
		dd->vertex(v[0], v[1], v[2], vcol);
	}
	dd->end();

	*/
}

void NavMesh::duDebugDrawNavMesh(SampleDebugDraw* dd, const dtNavMesh& mesh, unsigned char flags)
{
	if (!dd) return;

	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		drawMeshTile(dd, mesh, 0, tile, flags);
	}
}

void NavMesh::duDebugDrawNavMeshWithClosedList(SampleDebugDraw* dd, const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags)
{
	if (!dd) return;

	const dtNavMeshQuery* q = (flags & DU_DRAWNAVMESH_CLOSEDLIST) ? &query : 0;

	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		drawMeshTile(dd, mesh, q, tile, flags);
	}
}

