#include "PanelPopups.h"

#include "Helper/TagManager.h"

#include "Main/Application.h"
#include "Module/ModuleActions.h"
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
	RenderCreateScriptPopup();
	RenderAddTagPopup();

	resource_selector_popup.Render();
	scene_loader_popup.Render();
	scene_saver_popup.Render();
	new_filename_chooser.Render();
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
		//assert(progress <= 1);
		ImGui::ProgressBar(progress);

		if (App->resources->thread_comunication.finished_loading)
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void PanelPopups::RenderAddTagPopup()
{
	
	if (add_tag_popup_shown)
	{
		add_tag_popup_shown = false;
		ImGui::OpenPopup("Add Tag");
		App->actions->active_macros = true;
	}

	if (ImGui::BeginPopupModal("Add Tag", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		App->actions->active_macros = false;
		ImGui::Text("Tag Name: \n");
		ImGui::Separator();

		static char tag_name[128] = "";
		ImGui::InputTextWithHint("", "Enter tag name", tag_name, IM_ARRAYSIZE(tag_name));

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			std::string name = tag_name;
			if (name.size() > 0)
			{
				App->editor->tag_manager->AddTag(name);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void PanelPopups::RenderCreateScriptPopup()
{
	
	if (create_script_popup_shown)
	{
		create_script_popup_shown = false;
		ImGui::OpenPopup("Create Script");
		App->actions->active_macros = true;
	}

	if (ImGui::BeginPopupModal("Create Script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		App->actions->active_macros = false;
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