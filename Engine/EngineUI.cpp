#include "Globals.h"
#include "Application.h"
#include "EngineUI.h"
#include "EngineLog.h"
#include "imgui.h"

#include "ModuleWindow.h"
#include "SDL.h"

EngineUI::EngineUI()
{
}


EngineUI::~EngineUI()
{
}

const void EngineUI::ShowEngineUI()
{
	ShowConfigurationWindow();
	ShowDebugWindow();
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
	bool fullscreen = FULLSCREEN;
	bool bordered = false;
	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::Checkbox("Bordered", &bordered))
		{
			bordered = !bordered;
			if (bordered) {
				SDL_SetWindowBordered(App->window->window, SDL_TRUE);
			}
			else {
				SDL_SetWindowBordered(App->window->window, SDL_FALSE);
			}
		}
	}
	
}

const void EngineUI::ShowDebugWindow()
{
	if (ImGui::Begin("Debug"))
	{
		ShowConsole();
		ShowFPSGraph();
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

const void EngineUI::ShowFPSGraph()
{
	if (ImGui::CollapsingHeader("FPS")) {
		char title[25];
		std::vector<float> fps_log = App->log->getFPSData();
		if (fps_log.size() > 0)
		{
			sprintf_s(title, "Framerate %.1f", fps_log[fps_log.size() - 1]);
			ImGui::PlotHistogram("###framerate", &fps_log[0], fps_log.size(), 0, title, 0.f, 100.f, ImVec2(310, 100));
		}
	}
}
