#include "Globals.h"
#include "Application.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "UI/EngineUI.h"

#include <SDL/SDL.h>
#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <ImGuizmo.h>
#include <FontAwesome5/IconsFontAwesome5.h>
#include <FontAwesome5/IconsFontAwesome5Brands.h>
#include "Brofiler/Brofiler.h"

// Called before render is available
bool ModuleUI::Init()
{
	APP_LOG_SECTION("************ Module UI Init ************");

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

	editor_ui = new EngineUI();
	editor_ui->InitUI();

	APP_LOG_SUCCESS("IMGUI editor initialized correctly.");

	return true;
}

update_status ModuleUI::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	return update_status::UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleUI::Update()
{
	//ImGui::ShowStyleEditor();
	//ImGui::ShowDemoWindow();
	return update_status::UPDATE_CONTINUE;
}

void ModuleUI::Render()
{
	BROFILER_CATEGORY("Render UI", Profiler::Color::BlueViolet);
	editor_ui->ShowEngineUI();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Called before quitting
bool ModuleUI::CleanUp()
{
	delete editor_ui;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

ImFont* ModuleUI::GetFont(const Fonts & font) const
{
	ImGuiIO& io = ImGui::GetIO();
	return io.Fonts->Fonts[static_cast<int>(font)];
}


void ModuleUI::LoadFonts()
{
	ImGuiIO& io = ImGui::GetIO();

	// LOADING FONT AWESOME 5 (FONT_FA)

	static const ImWchar icons_ranges_fa[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;

	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAS, 12.f, &icons_config, icons_ranges_fa);

	// LOADING FONT AWESOME 5 REGULAR (FONT_FAR)
	io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAR, 12.f, &icons_config, icons_ranges_fa);

	// LOADING FONT AWESOME 5 BRANDS (FONT_FAB)
	io.Fonts->AddFontDefault();
	static const ImWchar icons_ranges_fab[] = { ICON_MIN_FAB, ICON_MAX_FAB, 0 };
	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAB, 12.f, &icons_config, icons_ranges_fab);
}

