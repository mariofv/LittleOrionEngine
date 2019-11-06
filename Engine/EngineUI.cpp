#include "Globals.h"
#include "Application.h"
#include "EngineUI.h"
#include "EngineLog.h"
#include "imgui.h"

#include "ModuleWindow.h"
#include "SDL.h"
#include <GL/glew.h>


EngineUI::EngineUI()
{
}

EngineUI::~EngineUI()
{
}

const void EngineUI::InitUI()
{
	ui_flags::show_configuration_window = false;
	ui_flags::show_debug_window = false;
	ui_flags::show_about_window = false;

	window_options::fullscreen = FULLSCREEN;
	window_options::bordered = BORDERED;
	window_options::resizable = RESIZABLE;
	SDL_GetWindowSize(App->window->window, &window_options::width, &window_options::height);
	window_options::brightness = SDL_GetWindowBrightness(App->window->window);
}

const void EngineUI::ShowEngineUI()
{
	ShowMainMenu();
	
	if (ui_flags::show_configuration_window)
	{
		ShowConfigurationWindow();
	}
	if (ui_flags::show_debug_window)
	{
		ShowDebugWindow();
	}
	if (ui_flags::show_about_window)
	{
		ShowAboutWindow();
	}
}

const void EngineUI::ShowMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::MenuItem("Config", (const char*)0, &ui_flags::show_configuration_window);
		ImGui::MenuItem("Debug", (const char*)0, &ui_flags::show_debug_window);
		ImGui::MenuItem("About", (const char*)0, &ui_flags::show_about_window);
		ImGui::EndMainMenuBar();
	}
	
}

const void EngineUI::ShowConfigurationWindow()
{
	if (ImGui::Begin("Configuration"))
	{
		ShowWindowOptions();
	}
	ImGui::End();
}

const void EngineUI::ShowWindowOptions()
{
	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::SliderFloat("Brightness", &window_options::brightness, 0, 1))
		{
			App->window->setBrightness(window_options::brightness);
		}

		if (ImGui::SliderInt("Width", &window_options::width, SCREEN_WIDTH, App->window->screen_width))
		{
			App->window->setWidth(window_options::width);
		}

		if (ImGui::SliderInt("Height", &window_options::height, SCREEN_HEIGHT, App->window->screen_height))
		{
			App->window->setHeight(window_options::height);
		}

		if (ImGui::Combo("Window style", &window_options::fullscreen, "Windowed\0Fullscreen desktop\0Fullscreen\0"))
		{
			switch (window_options::fullscreen)
			{
			case 0:
				App->window->setWindowed();
				break;
			case 1:
				App->window->setFullScreenDesktop();
				break;
			case 2:
				App->window->setFullScreen();
				break;
			}
		}

		if (ImGui::Checkbox("Bordered", &window_options::bordered))
		{
			App->window->setBordered(window_options::bordered);
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &window_options::resizable))
		{
			App->window->setResizable(window_options::resizable);
		}
	}
	
}

const void EngineUI::ShowDebugWindow()
{
	if (ImGui::Begin("Debug"))
	{
		ShowConsole();
		ShowPerformanceGraphs();
		ShowHardware();
	}
	ImGui::End();
}

const void EngineUI::ShowConsole()
{
	if (ImGui::CollapsingHeader("Console")) 
	{
		if (App->log->hasPendingData()) {
			ImGui::TextUnformatted(App->log->getData());
		}
	}
}

const void EngineUI::ShowPerformanceGraphs()
{
	if (ImGui::CollapsingHeader("Performance Graphs")) {
		ShowFPSGraph();
		ImGui::SameLine();
		ShowMSGraph();
	}
}

const void EngineUI::ShowFPSGraph()
{
	App->log->logFPS(ImGui::GetIO().Framerate);

	char title[25];
	std::vector<float> fps_log = App->log->getFPSData();
	if (fps_log.size() > 0)
	{
		sprintf_s(title, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("###framerate", &fps_log[0], fps_log.size(), 0, title, 0.f, 100.f, ImVec2(310, 100));
	}
}

const void EngineUI::ShowMSGraph()
{
	App->log->logMS(1000.f/ImGui::GetIO().Framerate);

	char title[25];
	std::vector<float> ms_log = App->log->getMSData();
	if (ms_log.size() > 0)
	{
		sprintf_s(title, "Miliseconds %.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("###miliseconds", &ms_log[0], ms_log.size(), 0, title, 0.f, 100.f, ImVec2(310, 100));
	}
}

const void EngineUI::ShowHardware()
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

const void EngineUI::ShowAboutWindow()
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


		ImGui::Separator();


		if (ImGui::MenuItem("LICENSE"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/mariofv/OrionEngine/blob/master/LICENSE", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::TextWrapped("Orion Engine is licensed under the MIT License, see LICENSE for more information.");
	}
	ImGui::End();
}
