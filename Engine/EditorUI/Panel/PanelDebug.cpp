#include "PanelDebug.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleSpacePartitioning.h"
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

		ImGui::Checkbox("Grid", &App->debug->show_grid);
		ImGui::Checkbox("Bounding boxes", &App->debug->show_bounding_boxes);
		ImGui::Checkbox("Global bounding boxes", &App->debug->show_global_bounding_boxes);
		ImGui::Checkbox("Camera Frustum", &App->debug->show_camera_frustum);
		ImGui::Checkbox("QuadTree", &App->debug->show_quadtree);
		ImGui::Checkbox("OctTree", &App->debug->show_octtree);
		ImGui::Checkbox("AABBTree", &App->debug->show_aabbtree);
		ImGui::Checkbox("Show Navmesh", &App->debug->show_navmesh);
		ImGui::Checkbox("Show Axis", &App->debug->show_axis);
		ImGui::Separator();

		ImGui::DragFloat("Rendering time ", &App->debug->rendering_time, NULL, NULL);
	}
	ImGui::End();
}