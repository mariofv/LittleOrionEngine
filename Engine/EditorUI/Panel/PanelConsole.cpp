#include "PanelConsole.h"

#include "Main/Application.h"
#include "Main/Globals.h"

#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>

#include <algorithm>

PanelConsole::PanelConsole()
{
	opened = true;
	enabled = true;
	window_name = ICON_FA_TERMINAL " Console";
}

void PanelConsole::Render()
{
	hovered = ImGui::IsWindowHovered();

	char tmp_string[64];
	if (ImGui::Begin(ICON_FA_TERMINAL " Console", &opened))
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Filters");
		ImGui::SameLine();
		ShowFilterButton("APP", ImVec4(0.3f, 0.3f, 0.3f, 1.0f), app_filter);
		ImGui::SameLine();
		ShowFilterButton("ASSIMP", ImVec4(0.8f, 0.32f, 0.04f, 1.0f), assimp_filter);
		ImGui::SameLine();
		ShowFilterButton("OPENGL", ImVec4(0.13f, 0.31f, 0.43f, 1.0f), opengl_filter);
		ImGui::Separator();

		if (ImGui::BeginChild(""))
		{
			/*
			for (auto& log_entry : App->engine_log->text_log)
			{
				if (
					(!app_filter && !assimp_filter && !opengl_filter)
					|| (app_filter && log_entry->source == EngineLog::LogEntrySource::APP_LOG)
					|| (assimp_filter && log_entry->source == EngineLog::LogEntrySource::ASSIMP_LOG)
					|| (opengl_filter && log_entry->source == EngineLog::LogEntrySource::OPENGL_LOG)
					)
				{
					switch (log_entry->source)
					{
					case EngineLog::LogEntrySource::APP_LOG:
						ImGui::Text("[APP]");
						ImGui::SameLine();
						break;
					case EngineLog::LogEntrySource::OPENGL_LOG:
						ImGui::TextColored(ImVec4(0.33f, 0.51f, 0.63f, 1.0f), "[OPENGL]");
						ImGui::SameLine();
						break;
					case EngineLog::LogEntrySource::ASSIMP_LOG:
						ImGui::TextColored(ImVec4(1.0f, 0.52f, 0.24f, 1.0f), "[ASSIMP]");
						ImGui::SameLine();
						break;
					default:
						break;
					}
					ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), log_entry->file);
					ImGui::SameLine();

					sprintf_s(tmp_string, "(%d):", log_entry->line);
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), tmp_string);
					ImGui::SameLine();

					switch (log_entry->type)
					{
					case EngineLog::LogEntryType::LOG_SECTION:
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), log_entry->message.c_str());
						break;
					case EngineLog::LogEntryType::LOG_INIT:
						ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), log_entry->message.c_str());
						break;
					case EngineLog::LogEntryType::LOG_SUCCESS:
						ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), log_entry->message.c_str());
						break;
					case EngineLog::LogEntryType::LOG_ERROR:
						ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), log_entry->message.c_str());
						break;
					default:
						ImGui::Text(log_entry->message.c_str());
						break;
					}
				}
			}
			*/
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


void PanelConsole::ShowFilterButton(const char* button_label, const ImVec4 & color, bool &filter)
{
	ImVec4 active_color = ImVec4(color.x, color.y + 0.3f, color.z + 0.3f, color.w);
	ImVec4 hovered_color = ImVec4(color.x, color.y + 0.2f, color.z + 0.2f, color.w);
	ImVec4 button_color = filter ? active_color : color;

	ImGui::PushStyleColor(ImGuiCol_Button, button_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hovered_color);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, active_color);

	if (ImGui::Button(button_label))
	{
		filter = !filter;
	}

	ImGui::PopStyleColor(3);
}