#include "Main/Application.h"
#include "ModuleAI.h"
#include "ModuleInput.h"


bool ModuleAI::Init()
{
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
