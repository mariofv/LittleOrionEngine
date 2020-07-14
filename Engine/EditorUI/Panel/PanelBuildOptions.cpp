#include "PanelBuildOptions.h"

#include "EditorUI/Panel/PanelPopups.h"
#include "Log/EngineLog.h"

#include "Main/Application.h"

#include "Module/ModuleEditor.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleResourceManager.h"

#include "ResourceManagement/Resources/Scene.h"
#include "ResourceManagement/Metafile/Metafile.h"

#include <FontAwesome5/IconsFontAwesome5.h>
#include <imgui.h>


PanelBuildOptions::PanelBuildOptions()
{
	window_name = ICON_FA_BUILDING " Building Options";
}

void PanelBuildOptions::Render()
{
	build_options_modified = false;

	if(ImGui::Begin(window_name.c_str(), &opened))
	{
		ImGui::Text("Choose Scenes to be load on a build.");

		unsigned number_of_scene = 1;
		for(auto& option : App->scene->build_options.get()->build_scenes)
		{
			ImGui::PushID(option.scene_uuid);
			ImGui::Text("%d- %s", number_of_scene, option.name.c_str());
			ImGui::SameLine();
			if(ImGui::Button("-"))
			{
				App->scene->build_options.get()->RemoveScene(number_of_scene - 1);
				build_options_modified = true;
				ImGui::PopID();
				break;
			}
			++number_of_scene;
			ImGui::PopID();
		}

		ImGui::Separator();

		if(ImGui::Button("Add current scene"))
		{
			if (App->scene->current_scene == nullptr)
			{
				if (!App->time->isGameRunning())
				{
					App->editor->popups->scene_saver_popup.popup_shown = true;
				}
				else
				{
					APP_LOG_INFO("You must stop play mode to save scene.");
				}
			}
			else
			{
				Metafile* scene_meta = App->resources->resource_DB->GetEntry(App->scene->current_scene->GetUUID());
				App->scene->build_options.get()->AddScene(scene_meta->uuid, scene_meta->imported_file_path, scene_meta->exported_file_path);
				build_options_modified = true;
			}
		}

		ImGui::Separator();
	}
	ImGui::End();

	if (build_options_modified)
	{
		App->scene->build_options.get()->SaveOptions();
	}
}