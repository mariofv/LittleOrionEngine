#include "NavMesh.h"

#include "Main/Application.h"
#include "UI/DebugDraw.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleRender.h"

#include "Component/ComponentMesh.h"
#include <math.h>
#include "recast/Detour/DetourNavMeshQuery.h"
#include "recast/Detour/DetourNavMeshBuilder.h"


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
	}
	return true;
}

bool NavMesh::CreateNavMesh()
{
	return true;
}

void NavMesh::GetVerticesScene()
{
}

void NavMesh::GetIndicesScene()
{
}

void NavMesh::GetNormalsScene()
{
}
