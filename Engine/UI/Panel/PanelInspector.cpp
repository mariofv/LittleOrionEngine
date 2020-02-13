#include "PanelInspector.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"

#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

void PanelInspector::Render()
{
	if (ImGui::Begin(ICON_FA_INFO_CIRCLE " Inspector"))
	{
		if (App->editor->selected_game_object != nullptr)
		{
			App->editor->selected_game_object->ShowPropertiesWindow();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ShowAddNewComponentButton();
		}

	}
	ImGui::End();
}


void PanelInspector::ShowAddNewComponentButton()
{
	float window_width = ImGui::GetWindowWidth();
	float button_width = 0.5f * window_width;
	ImGui::SetCursorPosX((window_width - button_width) / 2.f);
	ImGui::Button("Add component", ImVec2(button_width, 25));

	if (ImGui::BeginPopupContextItem("Add component", 0))
	{
		char tmp_string[128];

		sprintf_s(tmp_string, "%s Material", ICON_FA_IMAGE);
		if (ImGui::Selectable(tmp_string))
		{
			App->editor->selected_game_object->CreateComponent(Component::ComponentType::MATERIAL);

		}

		sprintf_s(tmp_string, "%s Camera", ICON_FA_VIDEO);
		if (ImGui::Selectable(tmp_string))
		{
			App->editor->selected_game_object->CreateComponent(Component::ComponentType::CAMERA);

		}

		sprintf_s(tmp_string, "%s Light", ICON_FA_LIGHTBULB);
		if (ImGui::Selectable(tmp_string))
		{
			App->editor->selected_game_object->CreateComponent(Component::ComponentType::LIGHT);

		}

		ImGui::EndPopup();
	}
}