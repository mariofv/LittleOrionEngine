#include "ModuleAI.h"

#include "Main/Application.h"
#include "ModuleInput.h"

#include <Brofiler/Brofiler.h>

bool ModuleAI::Init()
{
	nav_mesh.LoadNavMesh(nav_mesh_name);
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

ENGINE_API bool ModuleAI::IsPointWalkable(float3 & target_position)
{
	return nav_mesh.IsPointWalkable(target_position);
}

ENGINE_API bool ModuleAI::FindNextPolyByDirection(float3& position, float3& next_position)
{
	return nav_mesh.FindNextPolyByDirection(position, next_position);
}

void ModuleAI::SetNavMesh(const std::string nav_mesh_file)
{
	if(nav_mesh_name.size() > 1)
	{
		nav_mesh_name = nav_mesh_file;
	}
	nav_mesh.LoadNavMesh(nav_mesh_name);
}

std::string ModuleAI::GetNavMesh() const
{
	return nav_mesh_name;
}
