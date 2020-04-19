#include "PanelBuildOptions.h"

#include "Main/Application.h"
#include "Module/ModuleScene.h"
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
	if(ImGui::Begin(window_name.c_str(), &opened))
	{
		ImGui::Text("Choose Scenes to be load on a build.");

		unsigned number_of_scene = 1;
		for(auto& option : App->scene->build_options.get()->build_scenes)
		{
			ImGui::Text("%d- %s", number_of_scene, option.name.c_str());
			ImGui::SameLine();
			if(ImGui::Button("-"))
			{
				App->scene->build_options.get()->RemoveScene(number_of_scene - 1);
			}
			++number_of_scene;
		}

		ImGui::Separator();

		if(ImGui::Button("Add current scene"))
		{
			App->scene->build_options.get()->AddScene(App->scene->current_scene.get()->GetUUID(), App->scene->current_scene.get()->GetExportedFile(), App->scene->current_scene.get()->GetName());
		}

		ImGui::SameLine();

		if(ImGui::Button("+"))
		{
			//Open browser and get scene
			int i = 0;
		}

		ImGui::Separator();

		if(ImGui::Button("Save Build Options"))
		{
			App->scene->build_options.get()->SaveOptions();
		}
	}
	ImGui::End();
}