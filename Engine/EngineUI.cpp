#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleTime.h"
#include "ModuleModelLoader.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Model.h"
#include "EngineUI.h"
#include "EngineLog.h"

#include "imgui.h"
#include "SDL.h"
#include <GL/glew.h>
#include "IconsFontAwesome5.h"
#include "IconsFontAwesome5Brands.h"

void EngineUI::InitUI()
{
	show_scene_window = true;
	show_model_properties_window = true;
	show_configuration_window = true;
	show_console_window = true;
	show_about_window = false;

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigWindowsMoveFromTitleBarOnly = true;
}

void EngineUI::ShowEngineUI()
{
	ShowMainMenu();
	ShowTimeControls();

	if (show_scene_window)
	{
		ShowSceneWindow();
	}
	if (show_model_properties_window)
	{
		ShowModelPropertiesWindow();
	}
	if (show_configuration_window)
	{
		ShowConfigurationWindow();
	}
	if (show_console_window)
	{
		ShowConsoleWindow();
	}
	if (show_debug_window)
	{
		ShowDebugWindow();
	}
	if (show_about_window)
	{
		ShowAboutWindow();
	}
}

void EngineUI::ShowMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		ShowFileMenu();
		ShowViewMenu();
		ShowHelpMenu();
		ImGui::EndMainMenuBar();
	}	
}

void EngineUI::ShowFileMenu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem(ICON_FA_SIGN_OUT_ALT " Exit"))
		{
			SDL_Event quit_event;
			quit_event.type = SDL_QUIT;
			SDL_PushEvent(&quit_event);
		}

		ImGui::EndMenu();
	}
}

void EngineUI::ShowViewMenu()
{
	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem((ICON_FA_TH " Scene"), (const char*)0, &show_scene_window);
		ImGui::MenuItem((ICON_FA_CUBE " Properties"), (const char*)0, &show_model_properties_window);
		ImGui::MenuItem((ICON_FA_COGS " Config"), (const char*)0, &show_configuration_window);
		ImGui::MenuItem((ICON_FA_TERMINAL " Console"), (const char*)0, &show_console_window);
		ImGui::MenuItem((ICON_FA_BUG " Debug"), (const char*)0, &show_debug_window);

		ImGui::EndMenu();
	}
}

void EngineUI::ShowHelpMenu()
{
	if (ImGui::BeginMenu("Help"))
	{
		ImGui::MenuItem(ICON_FA_QUESTION_CIRCLE " About", (const char*)0, &show_about_window);
		ImGui::PushFont(App->editor->GetFont(Fonts::FONT_FAB));
		if (ImGui::MenuItem(ICON_FA_GITHUB_ALT " Repository"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/mariofv/OrionEngine/", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::PopFont();
		ImGui::EndMenu();
	}
}

void EngineUI::ShowTimeControls()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(ImVec2(0, MAIN_MENU_BAR_HEIGHT));
	ImGui::SetNextWindowSize(ImVec2(App->window->getWidth(), App->window->getHeight()*TIME_BAR_HEIGHT_PROP));
	App->time->ShowTimeControls();
	ImGui::PopStyleVar();
}

void EngineUI::ShowSceneWindow()
{
	ImGui::SetNextWindowPos(
		ImVec2(App->window->getWidth() * CONFIG_WIDTH_PROP, MAIN_MENU_BAR_HEIGHT + App->window->getHeight() * TIME_BAR_HEIGHT_PROP),
		ImGuiCond_Once
	);
	ImGui::SetNextWindowSize(
		ImVec2(App->window->getWidth() * SCENE_WIDTH_PROP, App->window->getHeight() * SCENE_HEIGHT_PROP),
		ImGuiCond_Once
	);
	App->scene->ShowSceneWindow();
}

void EngineUI::ShowModelPropertiesWindow()
{
	ImGui::SetNextWindowPos(
		ImVec2(App->window->getWidth() * (CONFIG_WIDTH_PROP + SCENE_WIDTH_PROP), MAIN_MENU_BAR_HEIGHT + App->window->getHeight() * TIME_BAR_HEIGHT_PROP),
		ImGuiCond_Once
	);
	ImGui::SetNextWindowSize(
		ImVec2(App->window->getWidth() * MODEL_PROPERTIES_WIDTH_PROP, App->window->getHeight() * MODEL_PROPERTIES_HEIGHT_PROP),
		ImGuiCond_Once
	);
	App->model_loader->current_model->ShowModelProperties();
}

void EngineUI::ShowConfigurationWindow()
{
	ImGui::SetNextWindowPos(
		ImVec2(0, MAIN_MENU_BAR_HEIGHT + App->window->getHeight() * TIME_BAR_HEIGHT_PROP),
		ImGuiCond_Once
	);
	ImGui::SetNextWindowSize(
		ImVec2(App->window->getWidth() * CONFIG_WIDTH_PROP, App->window->getHeight() * CONFIG_HEIGHT_PROP),
		ImGuiCond_Once
	);
	if (ImGui::Begin(ICON_FA_COGS " Configuration"))
	{
		ShowHardware();

		ImGui::Spacing();
		App->window->ShowWindowOptions();

		ImGui::Spacing();
		App->renderer->ShowRenderOptions();

		ImGui::Spacing();
		App->cameras->ShowCameraOptions();

		ImGui::Spacing();
		App->time->ShowTimeOptions();

		ImGui::Spacing();
		App->input->ShowInputOptions();
	}
	ImGui::End();
}

void EngineUI::ShowConsoleWindow()
{
	ImGui::SetNextWindowPos(
		ImVec2(0, MAIN_MENU_BAR_HEIGHT + App->window->getHeight() * (TIME_BAR_HEIGHT_PROP + CONFIG_HEIGHT_PROP)),
		ImGuiCond_Once
	);
	ImGui::SetNextWindowSize(
		ImVec2(App->window->getWidth(), App->window->getHeight() * (0.98 - (TIME_BAR_HEIGHT_PROP + CONFIG_HEIGHT_PROP))),
		ImGuiCond_Once
	);
	App->engine_log->ShowConsoleWindow();
}

void EngineUI::ShowHardware() const
{
	if (ImGui::CollapsingHeader(ICON_FA_HDD " Hardware"))
	{
		char tmp_string[4096];

		ImGui::Text("CPUs:");
		ImGui::SameLine();
		int num_cpus = SDL_GetCPUCount();
		int cpu_cache_line_size = SDL_GetCPUCacheLineSize();
		sprintf_s(tmp_string, "%d (Cache: %d bytes)", num_cpus, cpu_cache_line_size);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("System RAM:");
		ImGui::SameLine();
		float system_ram = SDL_GetSystemRAM()/1000.f;
		sprintf_s(tmp_string, "%.2f GB", system_ram);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("Caps:");
		if (SDL_Has3DNow()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "3DNow");
		}
		if (SDL_HasAVX()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX") ;
		}
		if (SDL_HasAVX2()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX2");
		}
		if (SDL_HasMMX()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "MMX") ;
		}
		if (SDL_HasRDTSC()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "RDTSC");
		}
		if (SDL_HasSSE()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE");
		}
		if (SDL_HasSSE2()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE2");
		}
		if (SDL_HasSSE3()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE3");
		}
		if (SDL_HasSSE41()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE41");
		}
		if (SDL_HasSSE42()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE42");
		}

		ImGui::Separator();

		ImGui::Text("GPU:");
		ImGui::SameLine();
		sprintf_s(tmp_string, "%s %s", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		int vram_budget, vram_available;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vram_available);

		ImGui::Text("VRAM Budget:");
		ImGui::SameLine();
		sprintf_s(tmp_string, "%.2f MB", vram_budget/1000.f);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("VRAM Usage:");
		ImGui::SameLine();
		sprintf_s(tmp_string, "%.2f MB", (vram_budget - vram_available) / 1000.f);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("VRAM Available:");
		ImGui::SameLine();
		sprintf_s(tmp_string, "%.2f MB", vram_available / 1000.f);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);
	}
}

void EngineUI::ShowDebugWindow()
{
	if (ImGui::Begin(ICON_FA_BUG " Debug"))
	{
		ImGui::Checkbox("Move model around", &App->renderer->model_movement);
		ImGui::Checkbox("Enable model Bounding Box", &App->renderer->bounding_box_visible);
	}
	ImGui::End();
}

void EngineUI::ShowAboutWindow()
{
	if (ImGui::Begin(ICON_FA_QUESTION_CIRCLE " About")) 
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Orion Engine");
		ImGui::TextWrapped("3D engine developed during the Master's Degree in AAA Videogames Development.");


		ImGui::Separator();


		ImGui::Text("Authors:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Mario Fernandez Villalba");
		

		ImGui::Separator();


		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Libraries");
		
		if (ImGui::MenuItem("SDL 2.0")) 
		{
			ShellExecuteA(NULL, "open", "https://www.libsdl.org/", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		if (ImGui::MenuItem("glew 2.1.0"))
		{
			ShellExecuteA(NULL, "open", "http://glew.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		if (ImGui::MenuItem("MathGeoLib 1.5"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		if (ImGui::MenuItem("ImGui 1.73"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		if (ImGui::MenuItem("DevIL 1.8"))
		{
			ShellExecuteA(NULL, "open", "http://openil.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		if (ImGui::MenuItem("assimp 5.0"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/assimp/assimp", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		if (ImGui::MenuItem("Font Awesome 5"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/FortAwesome/Font-Awesome", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		if (ImGui::MenuItem("Icon Font Cpp Headers"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/juliettef/IconFontCppHeaders", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}

		ImGui::Separator();


		if (ImGui::MenuItem("LICENSE"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/mariofv/OrionEngine/blob/master/LICENSE", NULL, NULL, SW_SHOWNORMAL);
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}
		ImGui::TextWrapped("Orion Engine is licensed under the MIT License, see LICENSE for more information.");
	}
	ImGui::End();
}
