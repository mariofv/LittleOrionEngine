#include "PanelDebug.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Module/ModuleUI.h"
#include "SpacePartition/OLQuadTree.h"
#include "Module/ModulePhysics.h"

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
		focused = ImGui::IsWindowFocused();

		ImGui::Checkbox("ImGui Demo", &App->debug->show_imgui_demo);
		ImGui::Checkbox("Bounding boxes", &App->debug->show_bounding_boxes);
		ImGui::Checkbox("Global bounding boxes", &App->debug->show_global_bounding_boxes);
		ImGui::Checkbox("Canvas", &App->debug->show_canvas);
		ImGui::Checkbox("Transform 2D", &App->debug->show_transform_2d);
		ImGui::Checkbox("Camera Frustum", &App->debug->show_camera_frustum);
		ImGui::Checkbox("QuadTree", &App->debug->show_quadtree);
		ImGui::Checkbox("OctTree", &App->debug->show_octtree);
		ImGui::Checkbox("AABBTree", &App->debug->show_aabbtree);
		ImGui::Checkbox("Show Navmesh", &App->debug->show_navmesh);
		ImGui::Checkbox("Show Axis", &App->debug->show_axis);
		ImGui::Checkbox("Show Physics", &App->debug->show_physics);
		ImGui::Separator();

		ImGui::DragFloat("Rendering time ", &App->debug->rendering_time, NULL, NULL);

		ImGui::Separator();
		ImGui::DragInt("Number of textures loaded:", &App->resources->loading_thread_communication.number_of_textures_loaded);

		int current = static_cast<int>(App->resources->loading_thread_communication.current_number_of_resources_loaded);
		ImGui::DragInt("Number of resources loaded:", &current);

		int total = static_cast<int>(App->resources->loading_thread_communication.total_number_of_resources_to_load);
		ImGui::DragInt("Number of resources to load:", &total);

		bool loading = App->resources->loading_thread_communication.loading;
		ImGui::Checkbox("Loading: ", &loading);

		if (ImGui::Button("Generate Loading Screen"))
		{
			App->scene->LoadLoadingScreen();
		}

		float time_meshes = App->resources->time_loading_meshes;
		ImGui::DragFloat("Time loading meshes:", &time_meshes);

		ImGui::Checkbox("Disable UI rendering", &App->ui->disable_ui_render);

	}
	ImGui::End();
}