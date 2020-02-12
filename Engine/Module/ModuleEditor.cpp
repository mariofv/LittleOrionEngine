#include "ModuleEditor.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleModelLoader.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"

#include "Helper/Config.h"
#include "UI/Panel/PanelScene.h"
#include "UI/Panel/PanelGame.h"
#include "UI/EngineUI.h"

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>
#include <Brofiler/Brofiler.h>


// Called before render is available
bool ModuleEditor::Init()
{
	APP_LOG_SECTION("************ Module Editor Init ************");
	
	APP_LOG_INIT("Initializing IMGUI editor");
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

	editor_ui = new EngineUI();
	panels.push_back(scene_panel = new PanelScene());
	panels.push_back(game_panel = new PanelGame());

	APP_LOG_SUCCESS("IMGUI editor initialized correctly.");

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	return update_status::UPDATE_CONTINUE;
}


update_status ModuleEditor::Update()
{
	//ImGui::ShowStyleEditor();
	//ImGui::ShowDemoWindow();

	static bool inital_scene_loaded = false;
	if (!inital_scene_loaded && App->model_loader->thread_comunication.finished_loading)
	{
		OpenScene(ASSIGNMENT_SCENE_PATH);
		inital_scene_loaded = true;
	}
	return update_status::UPDATE_CONTINUE;
}

void ModuleEditor::Render()
{
	BROFILER_CATEGORY("Render UI", Profiler::Color::BlueViolet);
	for (auto& panel : panels)
	{
		panel->Render();
	}

	editor_ui->ShowEngineUI();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// Called before quitting
bool ModuleEditor::CleanUp()
{
	delete editor_ui;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	remove(TMP_SCENE_PATH);

	return true;
}

void ModuleEditor::OpenScene(const std::string &path) const
{
	size_t readed_bytes;
	char* scene_file_data = App->filesystem->Load(path.c_str(), readed_bytes);
	std::string serialized_scene_string = scene_file_data;
	free(scene_file_data);

	Config scene_config(serialized_scene_string);
	App->scene->Load(scene_config);
}

void ModuleEditor::SaveScene(const std::string &path) const
{
	Config scene_config;
	App->scene->Save(scene_config);
	std::string serialized_scene_string;
	scene_config.GetSerializedString(serialized_scene_string);

	App->filesystem->Save(path.c_str(), serialized_scene_string.c_str(), serialized_scene_string.size() + 1);
}


void ModuleEditor::ShowGizmoControls()
{
	ImVec2 window_size = ImGui::GetWindowSize();

	ImVec2 translate_control_pos(10, (window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(translate_control_pos);
	if (ImGui::Button(ICON_FA_ARROWS_ALT, ImVec2(24, 24)))
	{
		gizmo_operation = ImGuizmo::TRANSLATE;
	}

	ImGui::SameLine();
	ImVec2 rotation_control_pos(36, (window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(rotation_control_pos);
	if (ImGui::Button(ICON_FA_SYNC_ALT, ImVec2(24, 24)))
	{
		gizmo_operation = ImGuizmo::ROTATE;
	}

	ImGui::SameLine();
	ImVec2 scale_control_pos(62, (window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(scale_control_pos);
	if (ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT, ImVec2(24, 24)))
	{
		gizmo_operation = ImGuizmo::SCALE;
	}
}


ImFont* ModuleEditor::GetFont(const Fonts & font) const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.Fonts->Fonts[static_cast<int>(font)];
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

