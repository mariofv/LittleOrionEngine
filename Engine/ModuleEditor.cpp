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

ModuleEditor::ModuleEditor()
{
}

// Destructor
ModuleEditor::~ModuleEditor()
{
}

// Called before render is available
bool ModuleEditor::Init()
{
	LOG("Initializing IMGUI editor");
	SDL_GLContext gl_context = ImGui::CreateContext();

	bool err = ImGui_ImplSDL2_InitForOpenGL(App->window->window, gl_context); 
	if (!err)
	{
		LOG("Error initializing IMGUI editor SDL2.")
		return false;
	}

	err = ImGui_ImplOpenGL3_Init("#version 330");
	if (!err)
	{
		LOG("Error initializing IMGUI editor OpenGL3.")
		return false;
	}

	// LOADING FONT AWESOME 5
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	io.Fonts->AddFontFromFileTTF("./resources/fonts/" FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icons_ranges);

	App->ui->InitUI();

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

