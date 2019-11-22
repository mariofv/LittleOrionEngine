#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "EngineUI.h"

#include "SDL.h"
#include <GL/glew.h>
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "IconsFontAwesome5.h"
#include "IconsFontAwesome5Brands.h"

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
	App->ui->InitUI();

	APP_LOG_SUCCESS("IMGUI editor initialized correctly.");

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleEditor::Update()
{
	App->ui->ShowEngineUI();
	//ImGui::ShowStyleEditor();
	//ImGui::ShowDemoWindow();
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

ImFont* ModuleEditor::GetFont(Fonts font)
{
	ImGuiIO& io = ImGui::GetIO();
	return io.Fonts->Fonts[static_cast<int>(font)];
}


void ModuleEditor::LoadFonts()
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

