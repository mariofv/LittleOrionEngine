#include "Globals.h"
#include "Application.h"
#include "Module/ModuleWindow.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleModelLoader.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleInput.h"
#include "Component/ComponentCamera.h"
#include "EngineUI.h"
#include "EngineLog.h"

#include "imgui.h"
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>

void EngineUI::InitUI()
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigWindowsMoveFromTitleBarOnly = true;
}

void EngineUI::ShowEngineUI()
{
	ShowMainMenu();
	ShowTimeControls();
	ShowMainViewWindow();
	
	if (show_model_inspector_window)
	{
		ShowInspectorWindow();
	}
	if (show_hierarchy_window)
	{
		ShowHierarchyWindow();
	}
	if (show_configuration_window)
	{
		ShowConfigurationWindow();
	}
	ShowFileAndConsoleWindows();

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
		ImGui::MenuItem((ICON_FA_SITEMAP " Hierarchy"), (const char*)0, &show_hierarchy_window);
		ImGui::MenuItem((ICON_FA_TH " Scene"), (const char*)0, &show_scene_tab);
		ImGui::MenuItem((ICON_FA_GHOST " Game"), (const char*)0, &show_game_tab);
		ImGui::MenuItem((ICON_FA_INFO " Inspector"), (const char*)0, &show_model_inspector_window);
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
			ShellExecuteA(NULL, "open", "https://github.com/mariofv/LittleOrionEngine/", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::PopFont();
		ImGui::EndMenu();
	}
}

void EngineUI::ShowTimeControls()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(ImVec2(0, MAIN_MENU_BAR_HEIGHT));
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth(), App->window->GetHeight()*TIME_BAR_HEIGHT_PROP));
	App->time->ShowTimeControls();
	ImGui::PopStyleVar();
}

void EngineUI::ShowHierarchyWindow()
{
	ImGui::SetNextWindowPos(
		ImVec2(0, MAIN_MENU_BAR_HEIGHT + App->window->GetHeight() * TIME_BAR_HEIGHT_PROP),
		ImGuiCond_Once
	);
	ImGui::SetNextWindowSize(
		ImVec2(App->window->GetWidth() * CONFIG_WIDTH_PROP, App->window->GetHeight() * CONFIG_HEIGHT_PROP),
		ImGuiCond_Once
	);
	App->scene->hierarchy.ShowHierarchyWindow();
}

void EngineUI::ShowMainViewWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
	ImGui::SetNextWindowPos(
		ImVec2(App->window->GetWidth() * CONFIG_WIDTH_PROP, MAIN_MENU_BAR_HEIGHT + App->window->GetHeight() * TIME_BAR_HEIGHT_PROP),
		ImGuiCond_Once
	);
	ImGui::SetNextWindowSize(
		ImVec2(App->window->GetWidth() * SCENE_WIDTH_PROP, App->window->GetHeight() * SCENE_HEIGHT_PROP),
		ImGuiCond_Once
	);

	if (ImGui::Begin("MainView", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::BeginTabBar("MainViewTabs");

		if (show_scene_tab)
		{
			ShowSceneTab();
		}
		if (show_game_tab)
		{
			ShowGameTab();
		}

		ImGui::EndTabBar();
	}
	ImGui::End();

	ImGui::PopStyleVar();
}

void EngineUI::ShowSceneTab()
{
	App->scene->ShowFrameBufferTab(App->cameras->scene_camera, ICON_FA_TH " Scene");
}

void EngineUI::ShowGameTab()
{
	App->scene->ShowFrameBufferTab(App->cameras->active_camera, ICON_FA_GHOST " Game");
}

void EngineUI::ShowInspectorWindow()
{
	ImGui::SetNextWindowPos(
		ImVec2(App->window->GetWidth() * (CONFIG_WIDTH_PROP + SCENE_WIDTH_PROP), MAIN_MENU_BAR_HEIGHT + App->window->GetHeight() * TIME_BAR_HEIGHT_PROP),
		ImGuiCond_Once
	);
	ImGui::SetNextWindowSize(
		ImVec2(App->window->GetWidth() * MODEL_PROPERTIES_WIDTH_PROP, App->window->GetHeight() * MODEL_PROPERTIES_HEIGHT_PROP),
		ImGuiCond_Once
	);

	if (ImGui::Begin(ICON_FA_INFO_CIRCLE " Inspector"))
	{
		if (App->scene->hierarchy.selected_game_object != nullptr)
		{
			App->scene->hierarchy.selected_game_object->ShowPropertiesWindow();
			
			Component * selected_camera_component = App->scene->hierarchy.selected_game_object->GetComponent(Component::ComponentType::CAMERA);
			if (selected_camera_component != nullptr) {
				ComponentCamera* selected_camera = static_cast<ComponentCamera*>(selected_camera_component);
				App->cameras->active_camera = selected_camera;
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ShowAddNewComponentButton();
		}
		
	}
	ImGui::End();
}

void EngineUI::ShowAddNewComponentButton() 
{
	float window_width = ImGui::GetWindowWidth();
	float button_width = 0.5f * window_width;
	ImGui::SetCursorPosX((window_width - button_width) / 2.f);
	ImGui::Button("Add component", ImVec2(button_width, 25));

	if (ImGui::BeginPopupContextItem("Add component", 0))
	{
		char tmp_string[128];
		
		sprintf_s(tmp_string, "%s Material", ICON_FA_IMAGE);
		if (ImGui::Selectable(tmp_string))
		{
			App->scene->hierarchy.selected_game_object->CreateComponent(Component::ComponentType::MATERIAL);

		}

		sprintf_s(tmp_string, "%s Camera", ICON_FA_VIDEO);
		if (ImGui::Selectable(tmp_string))
		{
			App->scene->hierarchy.selected_game_object->CreateComponent(Component::ComponentType::CAMERA);

		}

		ImGui::EndPopup();
	}
}

void EngineUI::ShowConfigurationWindow()
{
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

void EngineUI::ShowFileAndConsoleWindows()
{
	ImGui::SetNextWindowPos(
		ImVec2(0, MAIN_MENU_BAR_HEIGHT + App->window->GetHeight() * (TIME_BAR_HEIGHT_PROP + CONFIG_HEIGHT_PROP)),
		ImGuiCond_Once
	);
	ImGui::SetNextWindowSize(
		ImVec2(App->window->GetWidth(), App->window->GetHeight() * (0.98 - (TIME_BAR_HEIGHT_PROP + CONFIG_HEIGHT_PROP))),
		ImGuiCond_Once
	);
	if (ImGui::Begin("BottomTab", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar))
	{
		ImGui::BeginTabBar("BottomTab");
		if (show_file_explorer_window)
		{
			file_explorer_ui.ShowAssetsFolders();
		}
		if (show_console_window)
		{
			App->engine_log->ShowConsoleWindow();
		}
		ImGui::EndTabBar();
		ImGui::End();
	}

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