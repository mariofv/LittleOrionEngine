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

const void EngineUI::InitUI()
{
	window_options::fullscreen = FULLSCREEN;
	window_options::bordered = BORDERED;
	window_options::resizable = RESIZABLE;
	SDL_GetWindowSize(App->window->window, &window_options::width, &window_options::height);
	window_options::brightness = SDL_GetWindowBrightness(App->window->window);
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
	App->log->logFPS(ImGui::GetIO().Framerate);

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
