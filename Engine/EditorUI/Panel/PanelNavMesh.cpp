#include "AI/NavMesh.h"
#include "Main/Application.h"
#include "Module/ModuleAI.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"
#include "PanelNavMesh.h"
#include <imgui.h>

#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>

PanelNavMesh::PanelNavMesh()
{
	opened = false;
	enabled = false;
	window_name = "NavMesh";
}


void PanelNavMesh::Render()
{
	if (ImGui::Begin((ICON_FA_BRAIN " AI"), &opened))
	{
		if(first_time)
		{
			App->artificial_intelligence->nav_mesh.InitAABB();
			first_time = false;
		}

		hovered = ImGui::IsWindowHovered();

		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "NavMesh");
		

		ImGui::Text("Properties");
		ImGui::Separator();
		ImGui::Text("Rasterization");	
		ImGui::DragFloat("Cell Size", &new_nav_mesh.cell_width, 0.5f, 0.001f, 20.f);
		ImGui::DragFloat("Cell Height", &new_nav_mesh.cell_height, 0.5f, 0.001f, 50.f);
		int gw = 0, gh = 0;
		rcCalcGridSize(&new_nav_mesh.global_AABB.minPoint[0],
			&new_nav_mesh.global_AABB.maxPoint[0],
			new_nav_mesh.cell_width, &gw, &gh);

		ImGui::Text("Voxels %d x %d", gw, gh);
		
		ImGui::Text("");
		ImGui::Text("Agent");
		ImGui::SliderFloat("Agent Height", &new_nav_mesh.agent_height, 0.001f, 50.f);
		ImGui::SliderFloat("Agent Radius", &new_nav_mesh.agent_radius, 0.001f, 20.f);
		ImGui::SliderFloat("Agent Max Climb", &new_nav_mesh.agent_max_climb, 0.001f, 20.f);
		//walkableSlopeAngle	The maximum slope that is considered walkable. [Limits: 0 <= value < 90] [Units: Degrees]
		ImGui::SliderFloat("Walkable Slope Angle", &new_nav_mesh.walkable_slope_angle, 0.0f, 90.0f);
		
		ImGui::Text("");
		ImGui::Text("Region");
		ImGui::SliderInt("Region Min Size", &new_nav_mesh.region_min_size, 1, 20);
		ImGui::SliderInt("Region Merge Size", &new_nav_mesh.region_merge_size, 2, 100);

		ImGui::Text("");
		ImGui::Text("Polygonization");

		ImGui::SliderFloat("Edge Max Len", &new_nav_mesh.edge_max_len, 1.0f, 50.f);
		ImGui::SliderFloat("Edge Max Error", &new_nav_mesh.edge_max_error, 0.001f, 20.f);
		ImGui::SliderFloat("Verts Per Poly", &new_nav_mesh.verts_per_poly, 3.0f, 2000.f);
		
		ImGui::Text("");
		ImGui::Text("Detail Mesh");
		ImGui::SliderFloat("Detail Sample Distance", &new_nav_mesh.detail_sample_distance, 1.0f, 20.f);
		ImGui::SliderFloat("Detail Sample Max Error", &new_nav_mesh.detail_sample_max_error, 1.0f, 20.f);

		ImGui::Text("Selected Position: (%.3f,%.3f, %.3f)", App->editor->selected_position.x, App->editor->selected_position.y, App->editor->selected_position.z);

		ImGui::Separator();

		ImGui::Text("Build NavMesh time: %.3f ms", new_nav_mesh.time_to_build);

		if(ImGui::Button("Bake NavMesh"))
		{
			succes = new_nav_mesh.CreateNavMesh();
			baked = true;
		}

		if (!succes)
		{
			ImGui::Text("Error: Cannot create NavMesh.");
		}
		else if (succes && baked)
		{
			baked = false;
			App->artificial_intelligence->nav_mesh = new_nav_mesh;
			App->artificial_intelligence->nav_mesh_name = App->scene->GetCurrentSceneName();
			App->artificial_intelligence->nav_mesh.CreateNavMesh();
			ImGui::Text("NavMesh created succesfully.");
		}
		 
		ImGui::Separator();

		if(ImGui::Button("Attach Starting Point"))
		{
			App->artificial_intelligence->start_position = App->editor->selected_position;
			App->artificial_intelligence->start_initialized = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Attach End Point"))
		{
			App->artificial_intelligence->end_position = App->editor->selected_position;
			App->artificial_intelligence->end_initialized = true;
		}

		ImGui::Text("Starting Point: (%.3f,%.3f,%.3f)", App->artificial_intelligence->start_position.x, App->artificial_intelligence->start_position.y, App->artificial_intelligence->start_position.z);
		ImGui::Text("Ending Point: (%.3f,%.3f,%.3f)", App->artificial_intelligence->end_position.x, App->artificial_intelligence->end_position.y, App->artificial_intelligence->end_position.z);

		if(ImGui::Button("Build path from start to end"))
		{
			App->artificial_intelligence->FindPath();
		}

	}
	ImGui::End();
}
