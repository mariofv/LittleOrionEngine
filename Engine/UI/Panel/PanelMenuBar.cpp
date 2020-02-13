#include "PanelMenuBar.h"

#include "Main/Application.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleFileSystem.h"
#include "UI/Panel/PanelAbout.h"
#include "UI/Panel/PanelConfiguration.h"
#include "UI/Panel/PanelConsole.h"
#include "UI/Panel/PanelDebug.h"
#include "UI/Panel/PanelGame.h"
#include "UI/Panel/PanelHierarchy.h"
#include "UI/Panel/PanelInspector.h"
#include "UI/Panel/PanelProjectExplorer.h"
#include "UI/Panel/PanelScene.h"

#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>
#include <SDL/SDL.h>

void PanelMenuBar::Render()
{
	if (ImGui::BeginMainMenuBar())
	{
		ShowFileMenu();
		ShowViewMenu();
		ShowHelpMenu();
		ImGui::EndMainMenuBar();
	}
}


void PanelMenuBar::ShowFileMenu()
{
	if (ImGui::BeginMenu("File"))
	{

		if (ImGui::MenuItem(ICON_FA_FILE " New Scene"))
		{
			App->editor->OpenScene(DEFAULT_SCENE_PATH);
		}
		if (App->filesystem->Exists(SAVED_SCENE_PATH))
		{
			if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Load Scene"))
			{
				App->editor->OpenScene(SAVED_SCENE_PATH);
			}
		}
		if (ImGui::MenuItem(ICON_FA_SAVE " Save Scene"))
		{
			App->editor->SaveScene(SAVED_SCENE_PATH);
		}
		if (ImGui::MenuItem(ICON_FA_SIGN_OUT_ALT " Exit"))
		{
			SDL_Event quit_event;
			quit_event.type = SDL_QUIT;
			SDL_PushEvent(&quit_event);
		}

		ImGui::EndMenu();
	}
}

void PanelMenuBar::ShowViewMenu()
{
	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem((ICON_FA_SITEMAP " Hierarchy"), (const char*)0, App->editor->hierarchy->IsEnabled()))
		{
			App->editor->hierarchy->SwitchEnable();
		}
		if (ImGui::MenuItem((ICON_FA_TH " Scene"), (const char*)0, App->editor->scene_panel->IsEnabled()))
		{
			App->editor->scene_panel->SwitchEnable();
		}
		if (ImGui::MenuItem((ICON_FA_GHOST " Game"), (const char*)0, App->editor->game_panel->IsEnabled()))
		{
			App->editor->game_panel->SwitchEnable();
		}
		if (ImGui::MenuItem((ICON_FA_INFO " Inspector"), (const char*)0, App->editor->inspector->IsEnabled()))
		{
			App->editor->inspector->SwitchEnable();
		}
		if (ImGui::MenuItem((ICON_FA_COGS " Config"), (const char*)0, App->editor->configuration->IsEnabled()))
		{
			App->editor->configuration->SwitchEnable();
		}
		if (ImGui::MenuItem((ICON_FA_FOLDER_OPEN " Explorer"), (const char*)0, App->editor->project_explorer->IsEnabled()))
		{
			App->editor->project_explorer->SwitchEnable();
		}
		if (ImGui::MenuItem((ICON_FA_TERMINAL " Console"), (const char*)0, App->editor->console->IsEnabled()))
		{
			App->editor->console->SwitchEnable();
		}
		if (ImGui::MenuItem((ICON_FA_BUG " Debug"), (const char*)0, App->editor->debug_panel->IsEnabled()))
		{
			App->editor->debug_panel->SwitchEnable();
		}
		ImGui::EndMenu();
	}
}

void PanelMenuBar::ShowHelpMenu()
{
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem(ICON_FA_QUESTION_CIRCLE " About", (const char*)0, App->editor->about->IsEnabled()))
		{
			App->editor->about->SwitchEnable();
		}
		ImGui::PushFont(App->editor->GetFont(Fonts::FONT_FAB));
		if (ImGui::MenuItem(ICON_FA_GITHUB_ALT " Repository"))
		{
			ShellExecuteA(NULL, "open", "https://github.com/unnamed-company/LittleOrionEngine/", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::PopFont();
		ImGui::EndMenu();
	}
}