#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleTime.h"
#include "EngineUI.h"
#include "EngineLog.h"

#include "imgui.h"
#include "SDL.h"
#include <GL/glew.h>


EngineUI::EngineUI()
{
}

EngineUI::~EngineUI()
{
}

void EngineUI::InitUI()
{
	show_configuration_window = false;
	show_console_window = false;
	show_about_window = false;
}

void EngineUI::ShowEngineUI()
{
	ShowMainMenu();
	
	if (show_configuration_window)
	{
		ShowConfigurationWindow();
	}
	if (show_console_window)
	{
		ShowConsoleWindow();
	}
	if (show_about_window)
	{
		ShowAboutWindow();
	}

	App->time->ShowTimeControls();
}

void EngineUI::ShowMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::MenuItem("Config", (const char*)0, &show_configuration_window);
		ImGui::MenuItem("Console", (const char*)0, &show_console_window);
		ImGui::MenuItem("About", (const char*)0, &show_about_window);
		ImGui::EndMainMenuBar();
	}
	
}

void EngineUI::ShowConfigurationWindow()
{
	if (ImGui::Begin("Configuration"))
	{
		ShowHardware();
		App->window->ShowWindowOptions();
		App->cameras->ShowCameraOptions();
		App->time->ShowTimeOptions();
	}
	ImGui::End();
}

void EngineUI::ShowConsoleWindow()
{
	if (ImGui::Begin("Console")) 
	{
		if (App->engine_log->hasPendingData()) {
			ImGui::TextUnformatted(App->engine_log->getData());
		}
	}
	ImGui::End();
}

void EngineUI::ShowHardware()
{
	if (ImGui::CollapsingHeader("Hardware"))
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
	}
}

void EngineUI::ShowAboutWindow()
{
	if (ImGui::Begin("About")) 
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

		if (ImGui::MenuItem("glew 2.1.0"))
		{
			ShellExecuteA(NULL, "open", "http://glew.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
		}

		if (ImGui::MenuItem("MathGeoLib 1.5"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/juj/MathGeoLib", NULL, NULL, SW_SHOWNORMAL);
		}

		if (ImGui::MenuItem("ImGui 1.73"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/ocornut/imgui", NULL, NULL, SW_SHOWNORMAL);
		}

		if (ImGui::MenuItem("DevIL 1.8"))
		{
			ShellExecuteA(NULL, "open", "http://openil.sourceforge.net/", NULL, NULL, SW_SHOWNORMAL);
		}

		if (ImGui::MenuItem("assimp 3.3.1"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/assimp/assimp", NULL, NULL, SW_SHOWNORMAL);
		}


		ImGui::Separator();


		if (ImGui::MenuItem("LICENSE"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/mariofv/OrionEngine/blob/master/LICENSE", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::TextWrapped("Orion Engine is licensed under the MIT License, see LICENSE for more information.");
	}
	ImGui::End();
}
