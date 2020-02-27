#include "PanelPopups.h"

#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"

#include <FontAwesome5/IconsFontAwesome5.h>
#include <imgui.h>

PanelPopups::PanelPopups()
{
	opened = true;
	enabled = true;
	window_name = "Popups";
}

void PanelPopups::Render()
{
	RenderAssetsLoadingPopup();
	RenderModelSelectorPopup();
}

void PanelPopups::RenderAssetsLoadingPopup()
{
	if (!show_assets_loading_popup)
	{
		show_assets_loading_popup = true;
		ImGui::OpenPopup("Loading Assets");
	}

	if (ImGui::BeginPopupModal("Loading Assets", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text("Loading Assets");
		float progress = (float)App->resources->thread_comunication.loaded_items / App->resources->thread_comunication.total_items;
		ImGui::ProgressBar(progress);

		if (App->resources->thread_comunication.finished_loading)
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void PanelPopups::RenderModelSelectorPopup()
{
	if (show_mesh_selector_popup)
	{
		ImGui::OpenPopup("MeshSelectorPopup");
		show_mesh_selector_popup = false;
		mesh_selector_opened = true;
	}

	if (ImGui::BeginPopupModal("MeshSelectorPopup", &mesh_selector_opened))
	{
		ImGui::Text("Loading Assets");

		ImGui::EndPopup();
	}
}