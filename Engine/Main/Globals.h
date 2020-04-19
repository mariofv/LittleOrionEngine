#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include "EditorUI/EngineLog.h"

#include "imgui.h"

#include <windows.h>
#include <stdio.h>

#define APP_LOG_SECTION(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_SECTION, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_INFO(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_INIT(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_INIT, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_SUCCESS(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_SUCCESS, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_ERROR(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

#define OPENGL_LOG_INFO(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_INIT(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_INIT, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_SUCCESS(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_SUCCESS, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_ERROR(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

#define MYASSIMP_LOG_INFO(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_INIT(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_INIT, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_SUCCESS(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_SUCCESS, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_ERROR(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

void logentry(const EngineLog::LogEntrySource source, const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...);


static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

enum class update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

enum class Fonts
{
	FONT_FA = 0,
	FONT_FAR = 1,
	FONT_FAB = 2
};

// Paths -------
# define APP_ICON_PATH "Resources/Textures/engine_icon.png"

# define VIDEO_BILLBOARD_TEXTURE_PATH "Resources/Textures/video-solid.png"
# define LIGHT_BILLBOARD_TEXTURE_PATH "Resources/Textures/lightbulb-solid.png"

#if DEBUG
# define SCRIPTS_DLL_PATH "Resources/Scripts/GamePlaySystemDebug.dll"
# define SCRIPT_DLL_FILE "GamePlaySystemDebu_.dll"
#elif GAME
# define SCRIPTS_DLL_PATH "Resources/Scripts/GameplaySystem.dll"
# define SCRIPT_DLL_FILE "GameplaySystem.dll"
#else
# define SCRIPTS_DLL_PATH "Resources/Scripts/GamePlaySystemRelease.dll"
# define SCRIPT_DLL_FILE "GamePlaySystemReleas_.dll"
#endif

# define OPENGL_MAJOR_VERSION 4
# define OPENGL_MINOR_VERSION 0

#define FULLSCREEN false
#define RESIZABLE true
#define BORDERED true

#define VSYNC true
#define TITLE "Little Orion Engine"


#endif //_GLOBALS_H_