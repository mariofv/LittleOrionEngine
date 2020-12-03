#include "ModuleAI.h"

#include "Main/Application.h"
#include "ModuleInput.h"

#include <Brofiler/Brofiler.h>

bool ModuleAI::Init()
{
	nav_mesh.LoadNavMesh();
	return true;
}

update_status ModuleAI::Update()
{
	BROFILER_CATEGORY("Module AI Update", Profiler::Color::FireBrick);
	nav_mesh.Update();

	return update_status::UPDATE_CONTINUE;
}

void ModuleAI::RenderNavMesh(ComponentCamera& camera)
{
	BROFILER_CATEGORY("Module AI Render Nave mesh", Profiler::Color::FloralWhite);
	nav_mesh.RenderNavMesh(camera);
}

bool ModuleAI::FindPath()
{
	return nav_mesh.FindPath(start_position, end_position , debug_path, PathMode::FOLLOW_PATH);
}

bool ModuleAI::IsPointWalkable(float3 & target_position)
{
	return nav_mesh.IsPointWalkable(target_position);
}

bool ModuleAI::FindNextPolyByDirection(float3& position, float3& next_position)
{
	return nav_mesh.FindNextPolyByDirection(position, next_position);
}
