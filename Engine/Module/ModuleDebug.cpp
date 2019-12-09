#include "ModuleDebug.h"
#include "Application.h"

#include <GL/glew.h>
#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

// Called before render is available
bool ModuleDebug::Init()
{
	APP_LOG_SECTION("************ Module Debug Init ************");

	
	APP_LOG_SUCCESS("Module Debug initialized correctly.")

	return true;
}

update_status ModuleDebug::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleDebug::PostUpdate()
{
	
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleDebug::CleanUp()
{
	APP_LOG_INFO("Destroying Debug");
	
	return true;
}

void ModuleDebug::ShowDebugWindow()
{
	if (ImGui::Begin(ICON_FA_BUG " Debug"))
	{
		ImGui::Checkbox("Grid", &show_grid);
		ImGui::Checkbox("Bounding boxes", &show_bounding_boxes);
		ImGui::Checkbox("Camera Frustum", &show_camera_frustum);
		ImGui::Checkbox("QuadTree", &show_quadtree);
	}
	ImGui::End();
}