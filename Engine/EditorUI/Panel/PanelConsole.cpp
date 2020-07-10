#include "PanelConsole.h"

#include "Main/Application.h"
#include "Main/Globals.h"
#include "Module/ModuleResourceManager.h"

#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

#include <algorithm>

PanelConsole::PanelConsole()
{
	opened = true;
	enabled = true;
	window_name = ICON_FA_TERMINAL " Console";
	SetCurrentLog(CurrentLog::GAMELOG);
}

void PanelConsole::Render()
{
	hovered = ImGui::IsWindowHovered();
	focused = ImGui::IsWindowFocused();

	char tmp_string[64];
	if (ImGui::Begin(ICON_FA_TERMINAL " Console", &opened, ImGuiWindowFlags_MenuBar))
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::Selectable("Clear", false, 0, ImVec2(40,0)))
			{
				ClearCurrentLog();
			}

			if (ImGui::Selectable("Clear on Play", clear_on_play, 0, ImVec2(100, 0)))
			{
				clear_on_play = !clear_on_play;
			}

			if (ImGui::BeginMenu((current_log_name + " " ICON_FA_CARET_DOWN).c_str()))
			{
				if (ImGui::MenuItem("Game", NULL, current_log_name == "Game"))
				{
					SetCurrentLog(CurrentLog::GAMELOG);
				}
				if (ImGui::MenuItem("LittleOrion", NULL, current_log_name == "LittleOrion"))
				{
					SetCurrentLog(CurrentLog::LITTLEORION);
				}
				if (ImGui::MenuItem("OpenGL", NULL, current_log_name == "OpenGL"))
				{
					SetCurrentLog(CurrentLog::OPENGL);
				}
				if (ImGui::MenuItem("Assimp", NULL, current_log_name == "Assimp"))
				{
					SetCurrentLog(CurrentLog::ASSIMP);
				}
				if (ImGui::MenuItem("Resources", NULL, current_log_name == "Resources"))
				{
					SetCurrentLog(CurrentLog::RESOURCES);
				}
				if (ImGui::MenuItem("Debug", NULL, current_log_name == "Debug"))
				{
					SetCurrentLog(CurrentLog::DEBUGLOG);
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (ImGui::BeginChild(""))
		{
			for (auto& log_entry : current_sink->messages)
			{
				ImGui::Text(log_entry.c_str());
			}
			if (App->engine_log->scroll_down)
			{
				ImGui::SetScrollHere(1.0f);
				App->engine_log->scroll_down = false;
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
}

void PanelConsole::SetCurrentLog(CurrentLog current_log_type)
{
	this->current_log_type = current_log_type;
	switch (current_log_type)
	{
		case CurrentLog::GAMELOG:
			current_sink = App->engine_log->game_sink;
			current_log_name = "Game";
			break;
		case CurrentLog::LITTLEORION:
			current_sink = App->engine_log->little_orion_sink;
			current_log_name = "LittleOrion";
			break;
		case CurrentLog::ASSIMP:
			current_sink = App->engine_log->assimp_sink;
			current_log_name = "Assimp";
			break;
		case CurrentLog::OPENGL:
			current_sink = App->engine_log->opengl_sink;
			current_log_name = "OpenGL";
			break;
		case CurrentLog::RESOURCES:
			current_sink = App->engine_log->resources_sink;
			current_log_name = "Resources";
			break;
		case CurrentLog::DEBUGLOG:
			current_sink = App->engine_log->debug_sink;
			current_log_name = "Debug";
			break;
		default:
			break;
	}
}

void PanelConsole::ClearCurrentLog()
{
	current_sink->messages.clear();
}

void PanelConsole::ClearGameLog()
{
	App->engine_log->game_sink->messages.clear();
}
