#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "ModuleCamera.h"

#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>

// Called before render is available
bool ModuleWindow::Init()
{
	APP_LOG_SECTION("************ Module Window Init ************");

	APP_LOG_INIT("Init SDL window & surface.");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		APP_LOG_ERROR("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		SDL_DisplayMode DM;
		SDL_GetDesktopDisplayMode(0, &DM);
		screen_width = DM.w;
		screen_height = DM.h;
		
		//Create window
		width = screen_width * 0.9;
		height = screen_height * 0.9;
		uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

		if(FULLSCREEN)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if (RESIZABLE)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if (!BORDERED)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		InitOpenGLAttributes();

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			APP_LOG_ERROR("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			// Create OpenGL context
			glcontext = SDL_GL_CreateContext(window);
		}
	}

	brightness = 1;
	fullscreen = FULLSCREEN;
	bordered = BORDERED;
	resizable = RESIZABLE;

	APP_LOG_SUCCESS("SDL window & surface initialized correctly.");

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	APP_LOG_INFO("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);

		// Delete OpenGL context
		SDL_GL_DeleteContext(glcontext);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetResizable(const bool resizable) const
{
	if (resizable) {
		SDL_SetWindowResizable(window, SDL_TRUE);
	}
	else {
		SDL_SetWindowResizable(window, SDL_FALSE);
	}
}

void ModuleWindow::SetBordered(const bool bordered) const
{
	if (bordered) {
		SDL_SetWindowBordered(window, SDL_TRUE);
	}
	else {
		SDL_SetWindowBordered(window, SDL_FALSE);
	}
}

void ModuleWindow::SetWindowed() const
{
	SDL_SetWindowFullscreen(window, 0);

}

void ModuleWindow::SetFullScreenDesktop() const
{
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void ModuleWindow::SetFullScreen() const
{
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void ModuleWindow::SetHeight(const int height)
{
	this->height = height;
	SDL_SetWindowSize(window, width, height);
}

float ModuleWindow::GetHeight() const
{
	return height;
}

void ModuleWindow::SetWidth(const int width)
{
	this->width = width;
	SDL_SetWindowSize(window, width, height);
}

float ModuleWindow::GetWidth() const
{
	return width;
}

void ModuleWindow::SetBrightness(const float brightness) const
{
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::WindowResized(const unsigned width, const unsigned height)
{
	this->width = width;
	this->height = height;
}

void ModuleWindow::ShowWindowOptions()
{
	ImGui::PushFont(App->editor->GetFont(Fonts::FONT_FAR));
	if (ImGui::CollapsingHeader(ICON_FA_WINDOW_MAXIMIZE " Window")) {
		if (ImGui::SliderFloat("Brightness", &brightness, 0, 1))
		{
			SetBrightness(brightness);
		}

		if (ImGui::SliderInt("Width", &width, screen_width/4, screen_width))
		{
			SetWidth(width);
		}

		if (ImGui::SliderInt("Height", &height, screen_height/4, screen_height))
		{
			SetHeight(height);
		}

		if (ImGui::Combo("Window style", &fullscreen, "Windowed\0Fullscreen desktop\0Fullscreen\0"))
		{
			switch (fullscreen)
			{
			case 0:
				SetWindowed();
				break;
			case 1:
				SetFullScreenDesktop();
				break;
			case 2:
				SetFullScreen();
				break;
			}
		}

		if (ImGui::Checkbox("Bordered", &bordered))
		{
			App->window->SetBordered(bordered);
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &resizable))
		{
			App->window->SetResizable(resizable);
		}
	}
	ImGui::PopFont();

}

void ModuleWindow::InitOpenGLAttributes() const
{
	APP_LOG_INIT("Creating Glew Renderer context");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
}