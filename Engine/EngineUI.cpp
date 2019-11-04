#include "Globals.h"
#include "Application.h"
#include "EngineUI.h"
#include "EngineLog.h"
#include "imgui.h"

EngineUI::EngineUI()
{
}


EngineUI::~EngineUI()
{
}

const void EngineUI::ShowEngineUI()
{
	ShowLog();
	ShowAppInfo();
}

const void EngineUI::ShowLog()
{
	ImGui::Begin("Log");
	if (App->log->hasPendingData()) {
		ImGui::TextUnformatted(App->log->getData());
	}
	ImGui::End();
}

const void EngineUI::ShowAppInfo()
{
	ImGui::Begin("Application");
	ShowFPSGraph();
	ImGui::End();
}

const void EngineUI::ShowFPSGraph()
{
	char title[25];
	std::vector<float> fps_log = App->log->getFPSData();
	if (fps_log.size() > 0)
	{
		sprintf_s(title, "Framerate %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("###framerate", &fps_log[0], fps_log.size(), 0, title, 0.f, 100.f, ImVec2(310, 100));
	}
}
