#include "PanelPopups.h"

#include "Main/Application.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleScriptManager.h"

#include <FontAwesome5/IconsFontAwesome5.h>


PanelPopups::PanelPopups()
{
	opened = true;
	enabled = true;
	window_name = "Popups";
}

void PanelPopups::Render()
{
	RenderAssetsLoadingPopup();
	CreateScript();
	resource_selector_popup.Render();
	scene_management_popup.Render();
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

void PanelPopups::CreateScript()
{
	if (create_script_shown)
	{
		create_script_shown = false;
		ImGui::OpenPopup("Create Script");
	}

	if (ImGui::BeginPopupModal("Create Script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Script Name: \n");
		ImGui::Separator();

		static char str1[128] = "";
		ImGui::InputTextWithHint("", "enter script name here", str1, IM_ARRAYSIZE(str1));

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			std::string name = str1;
			if (name.size() > 0) 
			{
				App->scripts->CreateScript(name);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}