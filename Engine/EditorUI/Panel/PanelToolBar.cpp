#include "PanelToolBar.h"

#include "Helper/Timer.h"
#include "Main/Application.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleWindow.h"

#include <FontAwesome5/IconsFontAwesome5.h>
#include <imgui.h>

PanelToolBar::PanelToolBar()
{
	enabled = true;
	window_name = "Toolbar";
}

void PanelToolBar::Render()
{
	if (ImGui::BeginChild("Toolbar", ImVec2(0, App->window->GetHeight() * 0.025), false, ImGuiWindowFlags_NoDecoration))
	{
		hovered = ImGui::IsWindowHovered();

		ShowGizmoControls();
		ShowTimeControls();
	}
	ImGui::EndChild();
}

void PanelToolBar::ShowGizmoControls()
{
	ImVec2 window_size = ImGui::GetWindowSize();

	ImVec2 translate_control_pos(10, (window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(translate_control_pos);
	if (ImGui::Button(ICON_FA_ARROWS_ALT, ImVec2(24, 24)))
	{
		App->editor->gizmo_operation = ImGuizmo::TRANSLATE;
	}

	ImGui::SameLine();
	ImVec2 rotation_control_pos(36, (window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(rotation_control_pos);
	if (ImGui::Button(ICON_FA_SYNC_ALT, ImVec2(24, 24)))
	{
		App->editor->gizmo_operation = ImGuizmo::ROTATE;
	}

	ImGui::SameLine();
	ImVec2 scale_control_pos(62, (window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(scale_control_pos);
	if (ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT, ImVec2(24, 24)))
	{
		App->editor->gizmo_operation = ImGuizmo::SCALE;
	}
}

void PanelToolBar::ShowTimeControls()
{
	ImVec2 time_window_size = ImGui::GetWindowSize();

	ImVec2 play_button_pos((time_window_size.x - 24)*0.5f - 26, (time_window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(play_button_pos);
	if (App->time->game_time_clock->Started())
	{
		// CHANGE DEFAULT BUTTON COLOR TO SELECTED BUTTON COLOR
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5816f, 0.94f, 0.9804f));
		if (ImGui::Button(ICON_FA_PLAY, ImVec2(24, 24)))
		{
			App->time->Play();
		}
		ImGui::PopStyleColor();
	}
	else
	{
		if (ImGui::Button(ICON_FA_PLAY, ImVec2(24, 24)))
		{
			App->time->Play();
		}
	}

	ImGui::SameLine();

	ImVec2 pause_button_pos((time_window_size.x - 24)*0.5f, (time_window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(pause_button_pos);
	if (App->time->game_time_clock->IsPaused())
	{
		// CHANGE DEFAULT BUTTON COLOR TO SELECTED BUTTON COLOR
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5816f, 0.94f, 0.9804f));
		if (ImGui::Button(ICON_FA_PAUSE, ImVec2(24, 24)))
		{
			App->time->Pause();
		}
		ImGui::PopStyleColor();
	}
	else
	{
		if (ImGui::Button(ICON_FA_PAUSE, ImVec2(24, 24)))
		{
			App->time->Pause();
		}
	}

	ImGui::SameLine();

	ImVec2 step_button_pos((time_window_size.x - 24)*0.5f + 26, (time_window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(step_button_pos);
	if (ImGui::Button(ICON_FA_STEP_FORWARD, ImVec2(24, 24)))
	{
		App->time->StepFrame();
	}
}
