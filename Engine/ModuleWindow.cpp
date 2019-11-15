#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"

#include "imgui.h"

ModuleWindow::ModuleWindow()
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		width = SCREEN_WIDTH;
		height = SCREEN_HEIGHT;
		Uint32 flags = SDL_WINDOW_SHOWN |  SDL_WINDOW_OPENGL;

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

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			
			screen_surface = SDL_GetWindowSurface(window);

			// Create OpenGL context
			glcontext = SDL_GL_CreateContext(window);
		}
	}

	brightness = 1;
	fullscreen = FULLSCREEN;
	bordered = BORDERED;
	resizable = RESIZABLE;

	SDL_DisplayMode DM;
	SDL_GetDesktopDisplayMode(0, &DM);
	screen_width = DM.w;
	screen_height = DM.h;

	return ret;
}

update_status ModuleWindow::Update()
{
	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

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

void ModuleWindow::setResizable(const bool resizable) const
{
	if (resizable) {
		SDL_SetWindowResizable(window, SDL_TRUE);
	}
	else {
		SDL_SetWindowResizable(window, SDL_FALSE);
	}
}

void ModuleWindow::setBordered(const bool bordered) const
{
	if (bordered) {
		SDL_SetWindowBordered(window, SDL_TRUE);
	}
	else {
		SDL_SetWindowBordered(window, SDL_FALSE);
	}
}

void ModuleWindow::setWindowed() const
{
	SDL_SetWindowFullscreen(window, 0);

}

void ModuleWindow::setFullScreenDesktop() const
{
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void ModuleWindow::setFullScreen() const
{
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void ModuleWindow::setHeight(const int height)
{
	this->height = height;
	SDL_SetWindowSize(window, width, height);
}

float ModuleWindow::getHeight() const
{
	return height;
}

void ModuleWindow::setWidth(const int width)
{
	this->width = width;
	SDL_SetWindowSize(window, width, height);
}

float ModuleWindow::getWidth() const
{
	return width;
}

void ModuleWindow::setBrightness(const float brightness) const
{
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::WindowResized(const unsigned width, const unsigned height)
{
	this->width = width;
	this->height = height;
	App->cameras->SetAspectRatio((float)width / height);
}

void ModuleWindow::ShowWindowOptions()
{
	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::SliderFloat("Brightness", &brightness, 0, 1))
		{
			setBrightness(brightness);
		}

		if (ImGui::SliderInt("Width", &width, SCREEN_WIDTH, screen_width))
		{
			setWidth(width);
		}

		if (ImGui::SliderInt("Height", &height, SCREEN_HEIGHT, screen_height))
		{
			setHeight(height);
		}

		if (ImGui::Combo("Window style", &fullscreen, "Windowed\0Fullscreen desktop\0Fullscreen\0"))
		{
			switch (fullscreen)
			{
			case 0:
				setWindowed();
				break;
			case 1:
				setFullScreenDesktop();
				break;
			case 2:
				setFullScreen();
				break;
			}
		}

		if (ImGui::Checkbox("Bordered", &bordered))
		{
			App->window->setBordered(bordered);
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &resizable))
		{
			App->window->setResizable(resizable);
		}
	}

}