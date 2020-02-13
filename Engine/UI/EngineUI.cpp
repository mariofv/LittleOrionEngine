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

	ShowPopups();
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