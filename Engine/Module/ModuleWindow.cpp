#include "ModuleWindow.h"

#include "Log/EngineLog.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"

#include <SDL_image/SDL_image.h>

// Called before render is available
bool ModuleWindow::Init()
{
	APP_LOG_SECTION("************ Module Window Init ************");

	APP_LOG_INFO("Init SDL window & surface.");
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
		
		width = static_cast<int>(screen_width * 0.9f);
		height = static_cast<int>(screen_height * 0.9f);

		uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED;

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
			SDL_Surface* icon = IMG_Load(APP_ICON_PATH);
			SDL_SetWindowIcon(window, icon);

			// Create OpenGL context
			glcontext = SDL_GL_CreateContext(window);
		}
	}

	brightness = 1;
	fullscreen = FULLSCREEN;
	bordered = BORDERED;
	resizable = RESIZABLE;

	APP_LOG_INFO("SDL window & surface initialized correctly.");

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

bool ModuleWindow::IsFocused() const
{
	uint32_t window_flags = SDL_GetWindowFlags(window);
	return window_flags & SDL_WINDOW_MOUSE_FOCUS;
}

void ModuleWindow::SetResizable(bool resizable) const
{
	if (resizable) {
		SDL_SetWindowResizable(window, SDL_TRUE);
	}
	else {
		SDL_SetWindowResizable(window, SDL_FALSE);
	}
}

void ModuleWindow::SetBordered(bool bordered) const
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

void ModuleWindow::SetHeight(int height)
{
	this->height = height;
	SDL_SetWindowSize(window, width, height);
}

float ModuleWindow::GetHeight() const
{
	return static_cast<float>(height);
}

void ModuleWindow::SetWidth(int width)
{
	this->width = width;
	SDL_SetWindowSize(window, width, height);
}

float ModuleWindow::GetWidth() const
{
	return static_cast<float>(width);
}

void ModuleWindow::SetBrightness(float brightness) const
{
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::WindowResized(unsigned width, unsigned height)
{
	this->width = width;
	this->height = height;
}

void ModuleWindow::InitOpenGLAttributes() const
{
	APP_LOG_INFO("Creating Glew Renderer context");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, OPENGL_MAJOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, OPENGL_MINOR_VERSION);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
}
