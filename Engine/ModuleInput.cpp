#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "SDL.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"

ModuleInput::ModuleInput()
{}

// Destructor
ModuleInput::~ModuleInput()
{}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::Update()
{
	SDL_PumpEvents();

	update_status ret;

	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		// Esc button is pressed
		switch (event.type)
		{
		case SDL_QUIT:
			return UPDATE_STOP;
			break;

		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				App->window->WindowResized(event.window.data1, event.window.data2);
			break;
		}
	}

	keyboard = SDL_GetKeyboardState(NULL);

	if (keyboard[SDL_SCANCODE_Q]) 
	{
		App->cameras->MoveUp(0.1f);
	}

	if (keyboard[SDL_SCANCODE_E])
	{
		App->cameras->MoveDown(0.1f);
	}

	if (keyboard[SDL_SCANCODE_W])
	{
		App->cameras->MoveFoward(0.1f);
	}

	if (keyboard[SDL_SCANCODE_S])
	{
		App->cameras->MoveBackward(0.1f);
	}

	if (keyboard[SDL_SCANCODE_A])
	{
		App->cameras->MoveLeft(0.1f);
	}

	if (keyboard[SDL_SCANCODE_D])
	{
		App->cameras->MoveRight(0.1f);
	}

	if (keyboard[SDL_SCANCODE_UP])
	{
		App->cameras->RotatePitch(0.1f);
	}

	if (keyboard[SDL_SCANCODE_DOWN])
	{
		App->cameras->RotatePitch(-0.1f);
	}

	if (keyboard[SDL_SCANCODE_LEFT])
	{
		App->cameras->RotateYaw(0.1f);
	}

	if (keyboard[SDL_SCANCODE_RIGHT])
	{
		App->cameras->RotateYaw(-0.1f);
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}
