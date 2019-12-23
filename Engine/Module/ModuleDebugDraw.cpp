#include "ModuleDebugDraw.h"

// Called before render is available
bool ModuleDebugDraw::Init()
{
	APP_LOG_SECTION("************ Module Debug Draw Init ************");

	
	APP_LOG_SUCCESS("Module Debug Draw initialized correctly.")

	return true;
}

update_status ModuleDebugDraw::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleDebugDraw::PostUpdate()
{
	
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleDebugDraw::CleanUp()
{
	APP_LOG_INFO("Destroying Debug");
	
	return true;
}