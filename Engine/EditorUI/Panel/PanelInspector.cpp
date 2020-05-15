#include "PanelInspector.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleLight.h"

#include "PanelProjectExplorer.h"

#include <Brofiler/Brofiler.h>
#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelInspector::PanelInspector()
{
	opened = true;
	enabled = true;
	window_name = ICON_FA_INFO_CIRCLE " Inspector";
}

void PanelInspector::Render()
{
	BROFILER_CATEGORY("Render Inspector Panel", Profiler::Color::BlueViolet);

	if (ImGui::Begin(ICON_FA_INFO_CIRCLE " Inspector", &opened))
	{
		hovered = ImGui::IsWindowHovered();
		focused = ImGui::IsWindowFocused();

		if (App->editor->selected_game_object != nullptr && App->editor->show_game_object_inspector)
		{
			gameobject_panel.Render(App->editor->selected_game_object);
		}
		else if (App->editor->selected_meta_file != nullptr)
		{
			metafile_panel.Render(App->editor->selected_meta_file);
		}

	}

	ImGui::End();
}