#include "Globals.h"
#include "EngineLog.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"

#include <algorithm>

// TODO: Add function to LOG OpenGL errors
// TODO: Change default constructors to Class() = default

EngineLog::EngineLog()
{
}

EngineLog::~EngineLog()
{
	text_log.clear();
	fps_log.clear();
}


void EngineLog::log(const LogEntrySource source, const LogEntryType type, const char* file, const int line, const char* message)
{
	LogEntry *new_log_entry = new LogEntry(source, type, file, line, message);
	text_log.push_back(new_log_entry);
	scroll_down = true;
}

void EngineLog::logFPS(const float fps)
{
	if (fps_log.size() < 100)
	{
		fps_log.push_back(fps);
	}
	else 
	{
		rotate(fps_log.begin(), fps_log.begin() + 1, fps_log.end());
		fps_log[99] = fps;
	}
}

void EngineLog::logMS(const float ms)
{
	if (ms_log.size() < 100)
	{
		ms_log.push_back(ms);
	}
	else
	{
		rotate(ms_log.begin(), ms_log.begin() + 1, ms_log.end());
		ms_log[99] = ms;
	}
}

std::vector<float> EngineLog::getFPSData() const
{
	return fps_log;
}

std::vector<float> EngineLog::getMSData() const
{
	return ms_log;
}

void EngineLog::ShowConsoleWindow()
{
	char tmp_string[64];
	if (ImGui::Begin(ICON_FA_TERMINAL " Console"))
	{
		for (int i = 0; i < text_log.size(); ++i)
		{
			switch (text_log[i]->source)
			{
				case LogEntrySource::APP_LOG:
					ImGui::Text("[APP]");
					ImGui::SameLine();
					break;
				case LogEntrySource::OPENGL_LOG:
					ImGui::TextColored(ImVec4(0.33f, 0.51f, 0.63f, 1.0f), "[OPENGL]");
					ImGui::SameLine();
					break;
				case LogEntrySource::ASSIMP_LOG:
					ImGui::TextColored(ImVec4(1.0f, 0.52f, 0.24f, 1.0f), "[ASSIMP]");
					ImGui::SameLine();
					break;
				default:
					break;
			}
			ImGui::TextColored(ImVec4(0.7f,0.7f,0.7f,1.0f),text_log[i]->file);
			ImGui::SameLine();

			sprintf_s(tmp_string, "(%d):", text_log[i]->line);
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), tmp_string);
			ImGui::SameLine();

			switch (text_log[i]->type)
			{
				case LogEntryType::LOG_SECTION:
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), text_log[i]->message.c_str());
					break;
				case LogEntryType::LOG_INIT:
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), text_log[i]->message.c_str());
					break;
				case LogEntryType::LOG_SUCCESS:
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), text_log[i]->message.c_str());
					break;
				case LogEntryType::LOG_ERROR:
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), text_log[i]->message.c_str());
					break;
				default:
					ImGui::Text(text_log[i]->message.c_str());
					break;
			}
		}
		if (scroll_down)
		{
			//ImGui::SetScrollHere(1.0f);
			scroll_down = false;
		}
	}
	ImGui::End();
}