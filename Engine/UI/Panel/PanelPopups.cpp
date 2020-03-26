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

	fileDialog.SetTitle("Load Scene");
	fileDialog.SetTypeFilters({ ".scene", ".cpp" , ".h"});
}

void PanelPopups::Render()
{
	CreateScript();
	SaveScene();
	LoadScene();
	if (!assets_loading_popup_shown)
	{
		assets_loading_popup_shown = true;
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

void PanelPopups::SaveScene()
{
	if (save_scene_shown)
	{
		save_scene_shown = false;
		ImGui::OpenPopup("Save Scene");
	}

	if (ImGui::BeginPopupModal("Save Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Scene name: \n");
		ImGui::Separator();

		static char str1[128] = "";
		ImGui::InputTextWithHint("", "enter scene name here (XXXX.scene format)", str1, IM_ARRAYSIZE(str1));

		if (ImGui::Button("Save", ImVec2(120, 0)))
		{
			std::string name = str1;
			std::size_t found = name.find_last_of(".");
			if (found == std::string::npos || found == 0)
			{
				APP_LOG_ERROR("Invalid name for scene, it should be of format XXXX.scene");
				return;
			}
			std::string file_extension = name.substr(found + 1, name.length());
			if (name.size() > 0 && file_extension == "scene")
			{
				std::string filepath(SCENE_ROOT_PATH);
				filepath.append(name);
				App->editor->SaveScene(filepath);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

void PanelPopups::LoadScene()
{
	if (load_scene_shown)
	{
		load_scene_shown = false;
		fileDialog.Open();
	}


	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		APP_LOG_INFO("Opening %s scene.", fileDialog.GetSelected().string());
		App->editor->OpenScene(fileDialog.GetSelected().string());
		App->scene->current_scene_path = fileDialog.GetSelected().string();
		fileDialog.ClearSelected();
	}

}

inline std::uint32_t ImGui::FileBrowser::GetDrivesBitMask()
{
	DWORD mask = GetLogicalDrives();
	uint32_t ret = 0;
	for (int i = 0; i < 26; ++i)
	{
		if (!(mask & (1 << i)))
			continue;
		char rootName[4] = { static_cast<char>('A' + i), ':', '\\', '\0' };
		UINT type = GetDriveTypeA(rootName);
		if (type == DRIVE_REMOVABLE || type == DRIVE_FIXED)
			ret |= (1 << i);
	}
	return ret;
}
