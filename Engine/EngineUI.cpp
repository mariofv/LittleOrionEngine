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
	ShowConsole();
}

const void EngineUI::ShowConsole()
{
	ImGui::Begin("Console");
	if (App->log->hasPendingData()) {
		ImGui::TextUnformatted(App->log->getData());
	}
	ImGui::End();
}
