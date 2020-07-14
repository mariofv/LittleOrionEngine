#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#ifdef ENGINE_EXPORTS
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

#include <imgui.h>

#include <windows.h>
#include <stdio.h>


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

# define OPENGL_MAJOR_VERSION 4
# define OPENGL_MINOR_VERSION 0

#define FULLSCREEN false
#define RESIZABLE true
#define BORDERED true

#define VSYNC true

#if GAME
#define TITLE "Twinlight Souls"
#else
#define TITLE "Little Orion Engine"
#endif

#endif //_GLOBALS_H_