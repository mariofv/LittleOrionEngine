#include "PanelInspector.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"

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
	if (ImGui::Begin(ICON_FA_INFO_CIRCLE " Inspector", &opened))
	{
		hovered = ImGui::IsWindowHovered();

		if (App->editor->selected_game_object != nullptr)
		{
			gameobject_panel.Render(App->editor->selected_game_object);
		}

	}
	ImGui::End();
}