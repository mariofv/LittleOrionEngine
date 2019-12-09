#include "ModuleDebug.h"
#include "Application.h"
#include "GameObject.h"
#include "Module/ModuleModelLoader.h"

#include <random>
#include <ctime>
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

void ModuleDebug::CreateHousesRandom() const
{
	std::srand(std::time(nullptr)); // use current time as seed for random generator
	for (unsigned int i = 0; i < num_houses; ++i)
	{
		GameObject *loaded_house = App->model_loader->LoadModel(HOUSE_MODEL_PATH);
		int x = std::rand() % max_dispersion_x;
		int z = std::rand() % max_dispersion_z;
		loaded_house->transform.SetTranslation(float3(x, 0, z));
	}
}


void ModuleDebug::ShowDebugWindow()
{
	if (ImGui::Begin(ICON_FA_BUG " Debug"))
	{
		ImGui::Checkbox("Grid", &show_grid);
		ImGui::Checkbox("Bounding boxes", &show_bounding_boxes);
		ImGui::Checkbox("Camera Frustum", &show_camera_frustum);
		ImGui::Checkbox("QuadTree", &show_quadtree);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SliderInt("Number of houses", &num_houses, 0, 1000);
		ImGui::SliderInt("Dispersion X", &max_dispersion_x, 0, 1000);
		ImGui::SliderInt("Dispersion Z", &max_dispersion_z, 0, 1000);
		if (ImGui::Button("Create houses scene"))
		{
			CreateHousesRandom();
		}
	}
	ImGui::End();
}