#include "Main/Globals.h"
#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleModelLoader.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleWindow.h"
#include "Component/ComponentCamera.h"
#include "EngineUI.h"
#include "EngineLog.h"

#include "imgui.h"
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>


void EngineUI::ShowEngineUI()
{
	ShowSceneControls();

	ShowPopups();
}


void EngineUI::ShowSceneControls()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(ImVec2(0, MAIN_MENU_BAR_HEIGHT));
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth(), App->window->GetHeight()*TIME_BAR_HEIGHT_PROP));
	if (ImGui::Begin("Time Controls", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		App->editor->ShowGizmoControls();
		App->time->ShowTimeControls();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void EngineUI::ShowPopups() const
{
	static bool assets_loading_popup_shown = false;
	if (!assets_loading_popup_shown)
	{
		assets_loading_popup_shown = true;
		ImGui::OpenPopup("Loading Assets");
	}

	if (ImGui::BeginPopupModal("Loading Assets", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text("Loading Assets");
		float progress = (float)App->model_loader->thread_comunication.loaded_items / App->model_loader->thread_comunication.total_items;
		ImGui::ProgressBar(progress);

		if (App->model_loader->thread_comunication.finished_loading) 
		{
			ImGui::CloseCurrentPopup(); 
		}
		ImGui::EndPopup();
	}
}