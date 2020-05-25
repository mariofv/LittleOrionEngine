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
	BROFILER_CATEGORY("Module AI Update", Profiler::Color::Lavender);
	nav_mesh.Update();

	return update_status::UPDATE_CONTINUE;
}

bool ModuleAI::CleanUp()
{
	return true;
}

void ModuleAI::RenderNavMesh(ComponentCamera& camera)
{
	nav_mesh.RenderNavMesh(camera);
}

bool ModuleAI::FindPath()
{
	return nav_mesh.FindPath(start_position, end_position , debug_path, PathMode::FOLLOW_PATH);
}

ENGINE_API bool ModuleAI::IsPointWalkable(float3 & target_position)
{
	return nav_mesh.IsPointWalkable(target_position);
}

ENGINE_API bool ModuleAI::FindNextPolyByDirection(float3& position, float3& next_position)
{
	return nav_mesh.FindNextPolyByDirection(position, next_position);
}
