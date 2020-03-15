#include "ModuleAI.h"

#include "Main/Application.h"
#include "ModuleInput.h"


bool ModuleAI::Init()
{
	nav_mesh.LoadNavMesh();
	return true;
}

update_status ModuleAI::Update()
{
	nav_mesh.Update();

	if (App->input->GetKeyDown(KeyCode::P))
	{
		nav_mesh.CreateNavMesh();
	}

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
