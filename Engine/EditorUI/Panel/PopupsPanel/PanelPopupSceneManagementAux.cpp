#include "PanelPopupSceneManagementAux.h"

#include "Main/Application.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"

#include "ResourceManagement/Manager/SceneManager.h"

#include <Brofiler/Brofiler.h>


PanelPopupSceneManagementAux::PanelPopupSceneManagementAux()
{
	opened = false;
	enabled = true;

	save_scene_popup = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
	load_scene_popup = ImGui::FileBrowser();

	window_name = "Popups Scene Management";
}

void PanelPopupSceneManagementAux::Render()
{
	RenderSaveScene();
	RenderLoadScene();
}

void PanelPopupSceneManagementAux::RenderLoadScene()
{
	if (load_scene_shown)
	{
		load_scene_shown = false;
		load_scene_popup.Open();

		load_scene_popup.SetTitle("Load Scene");
		load_scene_popup.SetTypeFilters({ ".scene", ".cpp" , ".h" });
	}

	load_scene_popup.Display();

	if (load_scene_popup.HasSelected())
	{
		APP_LOG_INFO("Loading %s scene.", load_scene_popup.GetSelected().string());
		std::string local_path = load_scene_popup.GetSelected().string();
		std::size_t found = local_path.find("Assets");

		if (found == std::string::npos || found == 0)
		{
			APP_LOG_ERROR("Invalid name for scene.");
			return;
		}
		local_path = local_path.substr(found, local_path.length());
		//Replace \\ by /
		size_t index = 0;
		while (true)
		{
			/* Locate the substring to replace. */
			index = local_path.find("\\", index);
			if (index == std::string::npos) break;

			/* Make the replacement. */
			local_path.replace(index, 1, "/");

			/* Advance index forward so the next iteration doesn't pick it up as well. */
			index += 1;
		}

		App->editor->OpenScene(local_path);
		App->editor->current_scene_path = load_scene_popup.GetSelected().string();
		load_scene_popup.ClearSelected();
	}
}

void PanelPopupSceneManagementAux::RenderSaveScene()
{
	if (save_scene_shown)
	{
		save_scene_shown = false;
		save_scene_popup.Open();

		save_scene_popup.SetTitle("Save Scene");
		save_scene_popup.SetTypeFilters({ ".scene", ".cpp" , ".h" });
	}

	save_scene_popup.Display();

	if (save_scene_popup.HasSelected())
	{
		APP_LOG_INFO("Saving %s scene.", save_scene_popup.GetSelected().string());

		SceneManager::Create(save_scene_popup.GetSelected().string());
		App->editor->current_scene_path = save_scene_popup.GetSelected().string();
		save_scene_popup.ClearSelected();
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