#include "NavMesh.h"

#include "Main/Application.h"
#include "UI/DebugDraw.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleCamera.h"

#include "Component/ComponentMesh.h"
#include "Component/ComponentCamera.h"
#include <math.h>
#include "recast/Detour/DetourNavMeshQuery.h"
#include "recast/Detour/DetourNavMeshBuilder.h"
#include "recast/DebugUtils/DetourDebugDraw.h"
#include "recast/Detour/DetourNavMesh.h"

NavMesh::NavMesh()
{
	m_ctx = new rcContext();
	nav_query = dtAllocNavMeshQuery();
}

NavMesh::~NavMesh()
{
}

bool NavMesh::CleanUp()
{
	return true;
}

bool NavMesh::Update()
{
	if (is_mesh_computed)
	{
		// Draw bounds
		const float* bmin = &global_AABB.minPoint[0];
		const float* bmax = &global_AABB.maxPoint[0];
		dd::aabb(math::float3(bmin[0], bmin[1], bmin[2]), math::float3(bmax[0], bmax[1], bmax[2]), math::float3(1.0f, 0.0f, 0.0f));
		//RenderNavMesh(&m_dd, *nav_mesh, *nav_query, nav_mesh_draw_flags);
		//duDebugDrawNavMeshWithClosedList(&m_dd, *nav_mesh, *nav_query, nav_mesh_draw_flags);;
	}
	return true;
}

bool NavMesh::CreateNavMesh()
{
	CleanUp();

	GetVerticesScene();
	GetIndicesScene();
	GetNormalsScene();
	
	global_AABB.SetNegativeInfinity();

	for (auto & mesh : App->renderer->meshes)
	{
		float minX = std::fmin(mesh->owner->aabb.bounding_box.minPoint.x, global_AABB.minPoint.x);
		float minY = std::fmin(mesh->owner->aabb.bounding_box.minPoint.y, global_AABB.minPoint.y);
		float minZ = std::fmin(mesh->owner->aabb.bounding_box.minPoint.z, global_AABB.minPoint.z);
		

		float maxX = std::fmax(mesh->owner->aabb.bounding_box.maxPoint.x, global_AABB.maxPoint.x);
		float maxY = std::fmax(mesh->owner->aabb.bounding_box.maxPoint.y, global_AABB.maxPoint.y);
		float maxZ = std::fmax(mesh->owner->aabb.bounding_box.maxPoint.z, global_AABB.maxPoint.z);


		global_AABB.maxPoint = float3(maxX, maxY,maxZ);
		global_AABB.minPoint = float3(minX, minY, minZ);

	}


	const float* bmin = &global_AABB.minPoint[0];
	const float* bmax = &global_AABB.maxPoint[0];
	const float* verts = &verts_vec[0];
	const int nverts = verts_vec.size();
	const int* tris = &tris_vec[0];
	const int ntris = tris_vec.size() / 3;
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
	m_cfg.walkableHeight = (int)ceilf(agent_height / m_cfg.ch);
	m_cfg.walkableClimb = (int)floorf(agent_max_climb / m_cfg.ch);
	m_cfg.walkableRadius = (int)ceilf(agent_radius / m_cfg.cs);
	m_cfg.maxEdgeLen = (int)(edge_max_len / cell_width);
	m_cfg.maxSimplificationError = edge_max_error;
	m_cfg.minRegionArea = (int)rcSqr(region_min_size);		// Note: area = size*size
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

	// Reset build times gathering.
	m_ctx->resetTimers();

	// Start the build process.	
	m_ctx->startTimer(RC_TIMER_TOTAL);

	m_ctx->log(RC_LOG_PROGRESS, "Building navigation:");
	m_ctx->log(RC_LOG_PROGRESS, " - %d x %d cells", m_cfg.width, m_cfg.height);
	m_ctx->log(RC_LOG_PROGRESS, " - %.1fK verts, %.1fK tris", nverts / 1000.0f, ntris / 1000.0f);

	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.

	m_solid = rcAllocHeightfield();
	if (!m_solid)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
		return false;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[ntris];
	if (!m_triareas)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(m_triareas, 0, ntris * sizeof(unsigned char));
	rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
	if (!rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
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
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
		return false;
	}
	if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
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
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
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
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
			return false;
		}

		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
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
			return false;
		}
	}
	else // SAMPLE_PARTITION_LAYERS
	{
		// Partition the walkable surface into simple regions without holes.
		if (!rcBuildLayerRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea))
		{
			m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
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
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
		return false;
	}
	if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
		return false;
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
		return false;
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
		return false;
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh)
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
		return false;
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
	{
		m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
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
		unsigned char* navData = 0;
		int navDataSize = 0;

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

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
			return false;
		}

		nav_mesh = dtAllocNavMesh();
		if (!nav_mesh)
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
			return false;
		}

		dtStatus status;

		status = nav_mesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
			return false;
		}

		status = nav_query->init(nav_mesh, 2048);
		if (dtStatusFailed(status))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
			return false;
		}
	}

	m_ctx->stopTimer(RC_TIMER_TOTAL);
	

	//RenderNavMesh(verts, nverts, tris, ntris, normals, texScale);
	//my_debug_draw = new LOInterfaces();
	is_mesh_computed = true;
	duDebugDrawNavMeshWithClosedList(&m_dd, *nav_mesh, *nav_query, nav_mesh_draw_flags);

	return true;
}

bool NavMesh::RenderNavMesh(duDebugDraw* dd ,const dtNavMesh& mesh, const dtNavMeshQuery& query, unsigned char flags) const
{
	const dtNavMeshQuery* q = (flags & DU_DRAWNAVMESH_CLOSEDLIST) ? &query : 0;

	for (int i = 0; i < mesh.getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = mesh.getTile(i);
		if (!tile->header) continue;
		RenderTile(dd, mesh, q, tile, flags);
	}
	
	return true;
}

void NavMesh::RenderTile(duDebugDraw* dd, const dtNavMesh& mesh, const dtNavMeshQuery* query, const dtMeshTile* tile, unsigned char flags) const
{

	//BORDERS
	static const float thr = 0.01f*0.01f;
	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];

		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION) continue;

		const dtPolyDetail* pd = &tile->detailMeshes[i];

		for (int j = 0, nj = (int)p->vertCount; j < nj; ++j)
		{
			const float* v0 = &tile->verts[p->verts[j] * 3];
			const float* v1 = &tile->verts[p->verts[(j + 1) % nj] * 3];

			// Draw detail mesh edges which align with the actual poly edge.
			// This is really slow.
			for (int k = 0; k < pd->triCount; ++k)
			{
				const unsigned char* t = &tile->detailTris[(pd->triBase + k) * 4];
				const float* tv[3];
				for (int m = 0; m < 3; ++m)
				{
					if (t[m] < p->vertCount)
						tv[m] = &tile->verts[p->verts[t[m]] * 3];
					else
						tv[m] = &tile->detailVerts[(pd->vertBase + (t[m] - p->vertCount)) * 3];
				}
				for (int m = 0, n = 2; m < 3; n = m++)
				{
					if ((dtGetDetailTriEdgeFlags(t[3], n) & DT_DETAIL_EDGE_BOUNDARY) == 0)
						continue;

					if (DistancePtLine2d(tv[n], v0, v1) < thr &&
						DistancePtLine2d(tv[m], v0, v1) < thr)
					{
						dd::line(math::float3(tv[n]), math::float3(tv[m]), math::float3(1, 0, 0));
					}
				}
			}
		}
	}





	//POINTS
	for (int i = 0; i < tile->header->vertCount; ++i)
	{
		const float* v = &tile->verts[i * 3];
		dd::point(math::float3(v[0], v[1], v[2]), math::float3(1,0,0));
	}

	/*
	dtPolyRef base = mesh.getPolyRefBase(tile);

	int tileNum = mesh.decodePolyIdTile(base);
	const unsigned int tileColor = 1;
	
	dd->depthMask(false);

	dd->begin(DU_DRAW_TRIS);
	for (int i = 0; i < tile->header->polyCount; ++i)
	{
		const dtPoly* p = &tile->polys[i];
		if (p->getType() == DT_POLYTYPE_OFFMESH_CONNECTION)	// Skip off-mesh links.
			continue;
			
		const dtPolyDetail* pd = &tile->detailMeshes[i];

		unsigned int col;
		if (query && query->isInClosedList(base | (dtPolyRef)i))
			col = duRGBA(255,196,0,64);
		else
		{
			if (flags & DU_DRAWNAVMESH_COLOR_TILES)
				col = tileColor;
			else
				col = duTransCol(dd->areaToCol(p->getArea()), 64);
		}
		
		for (int j = 0; j < pd->triCount; ++j)
		{
			const unsigned char* t = &tile->detailTris[(pd->triBase+j)*4];
			for (int k = 0; k < 3; ++k)
			{
				if (t[k] < p->vertCount)
					dd->vertex(&tile->verts[p->verts[t[k]]*3], col);
				else
					dd->vertex(&tile->detailVerts[(pd->vertBase+t[k]-p->vertCount)*3], col);
			}
		}
	}
	dd->end();
	
	
	///LINES UNTIL HERE ARE FOR RENDERING THE NAVMESH (CODE ABOVE)

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

	dd->depthMask(true);
	*/
	return;
}

void NavMesh::RenderNavMesh(ComponentCamera& camera)
{
	if(is_mesh_computed)
		m_dd.drawMesh(camera);
}

void NavMesh::GetVerticesScene()
{
	//Clear vertex vector
	verts_vec.clear();

	for (auto mesh : App->renderer->meshes)
	{
		for (int i = 0; i < mesh->mesh_to_render.get()->vertices.size(); ++i)
		{
			float4 vertss(mesh->mesh_to_render.get()->vertices[i].position, 1.0f);
			vertss = mesh->owner->transform.GetGlobalModelMatrix() * vertss;

			verts_vec.push_back(vertss.x);
			verts_vec.push_back(vertss.y);
			verts_vec.push_back(vertss.z);
		}
	}
}

void NavMesh::GetIndicesScene()
{
	//Clear index vector
	tris_vec.clear();

	for (auto mesh : App->renderer->meshes)
	{
		for (int i = 0; i < mesh->mesh_to_render.get()->indices.size(); ++i)
		{
			tris_vec.push_back(mesh->mesh_to_render.get()->indices[i]);
		}
	}
}

void NavMesh::GetNormalsScene()
{
	//Clear normals vector
	normals_vec.clear();

	for (auto mesh : App->renderer->meshes)
	{
		for (int i = 0; i < mesh->mesh_to_render.get()->vertices.size(); ++i)
		{
			normals_vec.push_back(mesh->mesh_to_render.get()->vertices[i].normals.x);
			normals_vec.push_back(mesh->mesh_to_render.get()->vertices[i].normals.y);
			normals_vec.push_back(mesh->mesh_to_render.get()->vertices[i].normals.z);
		}
	}
}
