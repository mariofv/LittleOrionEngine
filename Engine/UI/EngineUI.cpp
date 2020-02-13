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
	if (0)
	{
		ShowAboutWindow();
	}

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

void EngineUI::ShowAboutWindow()
{
	if (ImGui::Begin(ICON_FA_QUESTION_CIRCLE " About")) 
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Orion Engine");
		ImGui::TextWrapped("3D engine developed during the Master's Degree in AAA Videogames Development.");


		ImGui::Separator();


		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Authors:");
		ImGui::Text("Mario Fernandez Villalba");
		ImGui::Text("Anabel Hernandez Barrera");
		

		ImGui::Separator();


		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Libraries");
		
		MenuURL("SDL 2.0", "https://www.libsdl.org/");
		MenuURL("glew 2.1.0", "http://glew.sourceforge.net/");
		MenuURL("MathGeoLib 1.5", "https://github.com/juj/MathGeoLib/");
		MenuURL("ImGui 1.73", "https://github.com/ocornut/imgui/");
		MenuURL("DevIL 1.8", "http://openil.sourceforge.net/");
		MenuURL("assimp 5.0", "https://github.com/assimp/assimp/");
		MenuURL("Font Awesome 5", "https://github.com/FortAwesome/Font-Awesome/");
		MenuURL("Icon Font Cpp Headers", "https://github.com/juliettef/IconFontCppHeaders/");
		MenuURL("PCG", "http://www.pcg-random.org/");
		MenuURL("rapidjson 1.1.0", "https://github.com/Tencent/rapidjson/");
		MenuURL("Debug Draw", "https://github.com/glampert/debug-draw/");
		MenuURL("par_shapes", "https://github.com/prideout/par/blob/master/par_shapes.h");
		MenuURL("ImGuizmo", "https://github.com/CedricGuillemet/ImGuizmo");
		MenuURL("PhysFS 3.0.2", "http://icculus.org/physfs/");

		ImGui::Separator();

		MenuURL("LICENSE", "https://github.com/mariofv/LittleOrionEngine/blob/master/LICENSE");
		ImGui::TextWrapped("Orion Engine is licensed under the MIT License, see LICENSE for more information.");
	}
	ImGui::End();
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


void EngineUI::MenuURL(const std::string& text, const std::string& link)
{
	if (ImGui::MenuItem(text.c_str()))
	{
		ShellExecuteA(NULL, "open", link.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	}
}