#include "PanelDebug.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleRender.h"
#include "SpacePartition/OLQuadTree.h"

#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelDebug::PanelDebug()
{
	window_name = ICON_FA_BUG " Debug";
}

void PanelDebug::Render()
{
	if (ImGui::Begin(ICON_FA_BUG " Debug", &opened))
	{
		hovered = ImGui::IsWindowHovered();

		ImGui::Checkbox("Grid", &App->debug->show_grid);
		ImGui::Checkbox("Bounding boxes", &App->debug->show_bounding_boxes);
		ImGui::Checkbox("Global bounding boxes", &App->debug->show_global_bounding_boxes);
		ImGui::Checkbox("Camera Frustum", &App->debug->show_camera_frustum);
		ImGui::Checkbox("QuadTree", &App->debug->show_quadtree);
		ImGui::Checkbox("AABBTree", &App->debug->show_aabbtree);
		ImGui::Separator();

		ImGui::Checkbox("Scene window culling", &App->debug->culling_scene_mode);
		int culling_mode_int = static_cast<int>(App->debug->culling_mode);
		if (ImGui::Combo("Culling Mode", &culling_mode_int, "None\0Frustum Culling\0QuadTree Culling\0OctTree\0AabbTree Culling\0Combined Culling"))
		{
			switch (culling_mode_int)
			{
			case 0:
				App->debug->culling_mode = ModuleDebug::CullingMode::NONE;
				break;
			case 1:
				App->debug->culling_mode = ModuleDebug::CullingMode::FRUSTUM_CULLING;
				break;
			case 2:
				App->debug->culling_mode = ModuleDebug::CullingMode::QUADTREE_CULLING;
				break;
			case 4:
				App->debug->culling_mode = ModuleDebug::CullingMode::AABBTREE_CULLING;
				break;
			case 5:
				App->debug->culling_mode = ModuleDebug::CullingMode::COMBINED_CULLING;
				break;
			}
		}

		ImGui::DragFloat("Rendering time ", &App->debug->rendering_time, NULL, NULL);

		if (ImGui::SliderInt("Quadtree Depth ", &App->renderer->ol_quadtree.max_depth, 1, 10)) {
			App->renderer->GenerateQuadTree();
		}
		if (ImGui::SliderInt("Quadtree bucket size ", &App->renderer->ol_quadtree.bucket_size, 1, 10)) {
			App->renderer->GenerateQuadTree();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::SliderInt("Number of houses", &App->debug->num_houses, 0, 1000);
		ImGui::SliderInt("Dispersion X", &App->debug->max_dispersion_x, 0, 1000);
		ImGui::SliderInt("Dispersion Z", &App->debug->max_dispersion_z, 0, 1000);
		if (ImGui::Button("Create houses scene"))
		{
			App->debug->CreateHousesRandom();
		}
	}
	ImGui::End();
}