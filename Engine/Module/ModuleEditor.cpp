#include "ModuleEditor.h"

#include "EditorUI/Panel/PanelAbout.h"
#include "EditorUI/Panel/PanelBuildOptions.h"
#include "EditorUI/Panel/PanelConfiguration.h"
#include "EditorUI/Panel/PanelConsole.h"
#include "EditorUI/Panel/PanelDebug.h"
#include "EditorUI/Panel/PanelGame.h"
#include "EditorUI/Panel/PanelMenuBar.h"
#include "EditorUI/Panel/PanelNavMesh.h"
#include "EditorUI/Panel/PanelHierarchy.h"
#include "EditorUI/Panel/PanelInspector.h"
#include "EditorUI/Panel/PanelPopups.h"
#include "EditorUI/Panel/PanelProjectExplorer.h"
#include "EditorUI/Panel/PanelResourceDatabase.h"
#include "EditorUI/Panel/PanelStateMachine.h"
#include "EditorUI/Panel/PanelScene.h"
#include "EditorUI/Panel/PanelToolBar.h"
#include "EditorUI/Panel/PanelTags.h"

#include "Filesystem/PathAtlas.h"
#include "Helper/Config.h"
#include "Helper/TagManager.h"

#include "Main/Application.h"
#include "ModuleActions.h"
#include "ModuleDebug.h"
#include "ModuleInput.h"
#include "ModuleResourceManager.h"
#include "ModuleScene.h"
#include "ModuleScriptManager.h"
#include "ModuleActions.h"
#include "ModuleWindow.h"

#include "ResourceManagement/Manager/SceneManager.h"

#include <Brofiler/Brofiler.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>

#include <GL/glew.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <SDL/SDL.h>

// Called before render is available
bool ModuleEditor::Init()
{
	APP_LOG_SECTION("************ Module Editor Init ************");
	bool ret = true;
	ret = InitImgui();

	menu_bar = new PanelMenuBar();
	toolbar = new PanelToolBar();
	panels.push_back(game_panel = new PanelGame());
	panels.push_back(scene_panel = new PanelScene());
	panels.push_back(inspector = new PanelInspector());
	panels.push_back(hierarchy = new PanelHierarchy());
	panels.push_back(console = new PanelConsole());
	panels.push_back(project_explorer = new PanelProjectExplorer());
	panels.push_back(debug_panel = new PanelDebug());
	panels.push_back(configuration = new PanelConfiguration());
	panels.push_back(about = new PanelAbout());
	panels.push_back(resource_database = new PanelResourceDatabase());
	panels.push_back(popups = new PanelPopups());
	panels.push_back(nav_mesh = new PanelNavMesh());
	panels.push_back(state_machine = new PanelStateMachine());
	panels.push_back(build_options = new PanelBuildOptions());
	panels.push_back(tags_panel = new PanelTags());

	tag_manager = new TagManager();
	tag_manager->LoadTags();

	return ret;
}

bool ModuleEditor::InitImgui()
{
	APP_LOG_INFO("Initializing IMGUI editor");
	SDL_GLContext gl_context = ImGui::CreateContext();

	bool err = ImGui_ImplSDL2_InitForOpenGL(App->window->window, gl_context);
	if (!err)
	{
		APP_LOG_ERROR("Error initializing IMGUI editor SDL2.")
			return false;
	}

	err = ImGui_ImplOpenGL3_Init("#version 330");
	if (!err)
	{
		APP_LOG_ERROR("Error initializing IMGUI editor OpenGL3.")
			return false;
	}

	LoadFonts();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigWindowsMoveFromTitleBarOnly = true;

	APP_LOG_INFO("IMGUI editor initialized correctly.");
	return true;
}

update_status ModuleEditor::PreUpdate()
{
	BROFILER_CATEGORY("Module Editor PreUpdate", Profiler::Color::HotPink);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	return update_status::UPDATE_CONTINUE;
}


update_status ModuleEditor::Update()
{
	BROFILER_CATEGORY("Module Editor Update", Profiler::Color::LightPink);
	static bool inital_scene_loaded = false;

#if GAME
	if (!inital_scene_loaded)
	{
		App->scene->LoadScene(0);
		inital_scene_loaded = true;
		return update_status::UPDATE_CONTINUE;
	}
#else
	if (!inital_scene_loaded && App->resources->thread_comunication.finished_loading)
	{
		App->scene->LoadScene(0);
		inital_scene_loaded = true;
	}
	if (App->debug->show_imgui_demo)
	{
		ImGui::ShowDemoWindow();
	}

#endif

	imgui_context = ImGui::GetCurrentContext();

	return update_status::UPDATE_CONTINUE;
}

void ModuleEditor::Render()
{
	BROFILER_CATEGORY("Module Editor Render", Profiler::Color::BlueViolet);
	
#if !GAME
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth(), App->window->GetHeight()));
	if (ImGui::Begin("MainWindow", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		menu_bar->Render();
		toolbar->Render();

		ImGui::Separator();

		RenderEditorDockspace();
	}
	ImGui::End();
#endif
	ImGui::Render();

	BROFILER_CATEGORY("Render ImGui Draws", Profiler::Color::BlueViolet);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ModuleEditor::RenderEditorDockspace()
{
	if (ImGui::BeginChild("DockSpace", ImVec2(0, 0), false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus))
	{
		editor_dockspace_id = ImGui::GetID("EditorDockspace");
		bool initialized = ImGui::DockBuilderGetNode(editor_dockspace_id) != NULL;

		ImGui::DockSpace(editor_dockspace_id);

		if (!initialized)
		{
			InitEditorDockspace();
		}

		for (auto& panel : panels)
		{
			if (panel->IsOpened())
			{
				panel->Render();
			}
		}
	}
	ImGui::EndChild();
}

void ModuleEditor::InitEditorDockspace()
{
	ImGui::DockBuilderRemoveNode(editor_dockspace_id); // Clear out existing layout
	ImGui::DockBuilderAddNode(editor_dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
	ImVec2 child_window_size = ImGui::GetWindowSize();
	ImGui::DockBuilderSetNodeSize(editor_dockspace_id, child_window_size);

	ImGuiID dock_main_id = editor_dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.

	ImGuiID dock_id_left;
	ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_id_left);

	ImGuiID dock_id_left_upper;
	ImGuiID dock_id_left_bottom = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.25f, NULL, &dock_id_left_upper);

	ImGuiID dock_id_left_upper_right;
	ImGuiID dock_id_left_upper_left = ImGui::DockBuilderSplitNode(dock_id_left_upper, ImGuiDir_Left, 0.20f, NULL, &dock_id_left_upper_right);

	ImGui::DockBuilderDockWindow(hierarchy->GetWindowName().c_str(), dock_id_left_upper_left);
	ImGui::DockBuilderDockWindow(scene_panel->GetWindowName().c_str(), dock_id_left_upper_right);
	ImGui::DockBuilderDockWindow(game_panel->GetWindowName().c_str(), dock_id_left_upper_right);
	ImGui::DockBuilderDockWindow(state_machine->GetWindowName().c_str(), dock_id_left_upper_right);
	ImGui::DockBuilderDockWindow(inspector->GetWindowName().c_str(), dock_id_right);
	ImGui::DockBuilderDockWindow(console->GetWindowName().c_str(), dock_id_left_bottom);
	ImGui::DockBuilderDockWindow(project_explorer->GetWindowName().c_str(), dock_id_left_bottom);

	ImGui::DockBuilderFinish(editor_dockspace_id);
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	remove(TMP_SCENE_PATH);

	delete menu_bar;
	delete toolbar;
	for (auto& panel : panels)
	{
		delete panel;
	}

	return true;
}

ImFont* ModuleEditor::GetFont(const Fonts & font) const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.Fonts->Fonts[static_cast<int>(font)];
}

ImGuiContext * ModuleEditor::GetImGuiContext() const
{
	return imgui_context;
}

void ModuleEditor::LoadFonts()
{
	ImGuiIO& io = ImGui::GetIO();

	// LOADING FONT AWESOME 5 (FONT_FA)
	io.Fonts->AddFontDefault();
	static const ImWchar icons_ranges_fa[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAS, 12.f, &icons_config, icons_ranges_fa);

	// LOADING FONT AWESOME 5 REGULAR (FONT_FAR)
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAR, 12.f, &icons_config, icons_ranges_fa);

	// LOADING FONT AWESOME 5 BRANDS (FONT_FAB)
	io.Fonts->AddFontDefault();
	static const ImWchar icons_ranges_fab[] = { ICON_MIN_FAB, ICON_MAX_FAB, 0 };
	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAB, 12.f, &icons_config, icons_ranges_fab);
}
