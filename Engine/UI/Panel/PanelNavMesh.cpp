#include "AI/NavMesh.h"
#include "Main/Application.h"
#include "Module/ModuleAI.h"
#include "PanelNavMesh.h"
#include <imgui.h>


PanelNavMesh::PanelNavMesh()
{
	opened = true;
	enabled = true;
	window_name = "NavMesh";
}


void PanelNavMesh::Render()
{
	if (ImGui::Begin(window_name.c_str(), &opened))
	{
		hovered = ImGui::IsWindowHovered();

		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "NavMesh");
		ImGui::TextWrapped("3D engine developed during the Master's Degree in AAA Videogames Development.");

		ImGui::Text("Variables");
		ImGui::Separator();

		ImGui::SliderFloat("Cell Width", &App->artificial_intelligence->nav_mesh.cell_width, 0.001f, 20.f);
		ImGui::SliderFloat("Cell Height", &App->artificial_intelligence->nav_mesh.cell_height, 0.001f, 20.f);
		ImGui::SliderFloat("Walkable Slope Angle", &App->artificial_intelligence->nav_mesh.walkable_slope_angle, 0.0f, 360.f);
		ImGui::SliderFloat("Agent Height", &App->artificial_intelligence->nav_mesh.agent_height, 0.001f, 50.f);
		ImGui::SliderFloat("Agent Max Climb", &App->artificial_intelligence->nav_mesh.agent_max_climb, 0.001f, 20.f);
		ImGui::SliderFloat("Agent Radius", &App->artificial_intelligence->nav_mesh.agent_radius, 0.001f, 20.f);
		ImGui::SliderFloat("Edge Max Len", &App->artificial_intelligence->nav_mesh.edge_max_len, 1.0f, 50.f);
		ImGui::SliderFloat("Edge Max Error", &App->artificial_intelligence->nav_mesh.edge_max_error, 0.001f, 20.f);
		ImGui::SliderInt("Region Min Size", &App->artificial_intelligence->nav_mesh.region_min_size, 1, 20);
		ImGui::SliderInt("Region Merge Size", &App->artificial_intelligence->nav_mesh.region_merge_size, 2, 100);
		ImGui::SliderFloat("Verts Per Poly", &App->artificial_intelligence->nav_mesh.verts_per_poly, 3.0f, 2000.f);
		ImGui::SliderFloat("Detail Sample Distance", &App->artificial_intelligence->nav_mesh.detail_sample_distance, 1.0f, 20.f);
		ImGui::SliderFloat("Detail Sample Max Error", &App->artificial_intelligence->nav_mesh.detail_sample_max_error, 1.0f, 20.f);

		ImGui::Separator();

		if(ImGui::Button("Bake NavMesh"))
		{
			App->artificial_intelligence->nav_mesh.CreateNavMesh();
		}


	}
	ImGui::End();
}
