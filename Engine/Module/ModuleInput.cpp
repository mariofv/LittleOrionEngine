#include "ModuleInput.h"

#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleWindow.h"
#include "ModuleModelLoader.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "Component/ComponentCamera.h"
#include "UI/Panel/PanelProjectExplorer.h"
#include "UI/Panel/PanelScene.h"

#include <SDL/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include <GL/glew.h>
#include "Brofiler/Brofiler.h"

// Called before render is available
bool ModuleInput::Init()
{
	APP_LOG_SECTION("************ Module Input Init ************");

	APP_LOG_INIT("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		APP_LOG_ERROR("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		key_bible[(KeyCode)i] = KeyState::IDLE;
	}

	for (int i = 0; i < MAX_MOUSE_BUTTONS; ++i)
	{
		mouse_bible[(MouseButton)i] = KeyState::IDLE;
	}

	APP_LOG_SUCCESS("SDL input event system initialized correctly.");
	
	//Load Game Inputs
	size_t readed_bytes;
	char* scene_file_data = App->filesystem->Load(GAME_INPUT_PATH, readed_bytes);
	if(scene_file_data != nullptr)
	{
		std::string serialized_scene_string = scene_file_data;
		free(scene_file_data);

		Config input_config(serialized_scene_string);
		LoadGameInputs(input_config);	
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
	BROFILER_CATEGORY("Inputs PreUpdate", Profiler::Color::BlueViolet);

	mouse_motion = { 0, 0 };
	mouse_wheel_motion = 0;

	for (auto& mouse : mouse_bible)
	{
		if (mouse.second == KeyState::DOWN)
		{
			mouse.second = KeyState::REPEAT;
		}
		else if (mouse.second == KeyState::UP)
		{
			mouse.second = KeyState::IDLE;
		}
	}

	SDL_Event event;

	while (SDL_PollEvent(&event) != 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		switch (event.type)
		{
		case SDL_QUIT:
			return update_status::UPDATE_STOP;

		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				App->window->WindowResized(event.window.data1, event.window.data2);
			break;

		case SDL_MOUSEMOTION:
			mouse_position = float2(event.motion.x, event.motion.y);
			mouse_motion = float2(event.motion.xrel, event.motion.yrel);
			mouse_moving = event.motion.state & SDL_BUTTON_RMASK;
			break;

		case SDL_MOUSEWHEEL:
			mouse_wheel_motion = event.wheel.y;
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouse_bible[(MouseButton)event.button.button] = KeyState::DOWN;
			mouse_clicks = event.button.clicks;
			break;

		case SDL_MOUSEBUTTONUP:
			mouse_bible[(MouseButton)event.button.button] = KeyState::UP;
			break;

		case SDL_DROPFILE:
			char* dropped_filedir = event.drop.file;
			App->editor->project_explorer->CopyFileToSelectedFolder(dropped_filedir);
			SDL_free(dropped_filedir);
			break;
		}
	}

	keys = SDL_GetKeyboardState(nullptr);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (key_bible[(KeyCode)i] == KeyState::IDLE)
			{
				key_bible[(KeyCode)i] = KeyState::DOWN;
			}
			else if (key_bible[(KeyCode)i] == KeyState::DOWN)
			{
				key_bible[(KeyCode)i] = KeyState::REPEAT;
			}
		}
		else if (keys[i] == 0)
		{
			if (key_bible[(KeyCode)i] == KeyState::REPEAT || key_bible[(KeyCode)i] == KeyState::DOWN)
			{
				key_bible[(KeyCode)i] = KeyState::UP;
			}
			else if (key_bible[(KeyCode)i] == KeyState::UP)
			{
				key_bible[(KeyCode)i] = KeyState::IDLE;
			}
		}
	}

	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	APP_LOG_INFO("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

// Returns true while the user holds down the key identified by name
bool ModuleInput::GetKey(KeyCode key)
{
	return key_bible[key] == KeyState::REPEAT;
}

// Returns true during the frame the user starts pressing down the key identified by name
bool ModuleInput::GetKeyDown(KeyCode key)
{
	return key_bible[key] == KeyState::DOWN;
}

// Returns true during the frame the user releases the key identified by name
bool ModuleInput::GetKeyUp(KeyCode key)
{
	return key_bible[key] == KeyState::UP;
}

// Returns whether the given mouse button is held down
bool ModuleInput::GetMouseButton(MouseButton button)
{
	return mouse_bible[button] == KeyState::REPEAT;
}

// Returns true during the frame the user pressed the given mouse button
bool ModuleInput::GetMouseButtonDown(MouseButton button)
{
	return mouse_bible[button] == KeyState::DOWN;
}

// Returns true during the frame the user releases the given mouse button
bool ModuleInput::GetMouseButtonUp(MouseButton button)
{
	return mouse_bible[button] == KeyState::UP;
}

bool ModuleInput::GetGameInput(const char* name)
{
	GameInput button = game_inputs[name];
	for (auto &key : button.keys)
	{
		if (GetKey(key))
			return true;
	}

	for (auto &mouse : button.mouse_buttons)
	{
		if (GetMouseButton(mouse))
			return true;
	}

	return false;
}

bool ModuleInput::GetGameInputDown(const char* name)
{
	GameInput button = game_inputs[name];
	for (auto &key : button.keys)
	{
		if (GetKeyDown(key))
			return true;
	}

	for (auto &mouse : button.mouse_buttons)
	{
		if (GetMouseButtonDown(mouse))
			return true;
	}

	return false;
}

bool ModuleInput::GetGameInputUp(const char* name)
{
	GameInput button = game_inputs[name];
	for (auto &key : button.keys)
	{
		if (GetKeyUp(key))
			return true;
	}

	for (auto &mouse : button.mouse_buttons)
	{
		if (GetMouseButtonUp(mouse))
			return true;
	}

	return false;
}

void ModuleInput::CreateGameInput(GameInput game_input)
{
	game_inputs[game_input.name] = game_input;

	Config config;
	SaveGameInputs(config);

	std::string serialized_game_input_string;
	config.GetSerializedString(serialized_game_input_string);

	App->filesystem->Save(GAME_INPUT_PATH, serialized_game_input_string.c_str(), serialized_game_input_string.size() + 1);
}

// Returns the current mouse position in pixel coordinates
float2 ModuleInput::GetMousePosition() const
{
	return mouse_position;
}

// Returns the current mouse motion in relative coordinates
float2 ModuleInput::GetMouseMotion() const
{
	return mouse_motion;
}

// Returns the current mouse wheel motion (forward or backward)
Sint32 ModuleInput::GetMouseWheelMotion() const
{
	return mouse_wheel_motion;
}

Uint8 ModuleInput::GetMouseClicks() const
{
	return mouse_clicks;
}

// Returns if the mouse is currently being moved
bool ModuleInput::IsMouseMoving() const
{
	return mouse_moving;
}

void ModuleInput::SaveGameInputs(Config &config)
{
	std::vector<Config> game_inputs_configs;

	for(auto game_input : game_inputs)
	{
		Config game_inputs_config;
		game_input.second.Save(game_inputs_config);
		game_inputs_configs.push_back(game_inputs_config);
	}

	config.AddChildrenConfig(game_inputs_configs, "GameInputs");
}

void ModuleInput::LoadGameInputs(Config &serialized_config)
{
	std::vector<Config> game_inputs_configs;
	serialized_config.GetChildrenConfig("GameInputs", game_inputs_configs);
	for (unsigned int i = 0; i < game_inputs_configs.size(); ++i)
	{
		GameInput game_input;
		game_input.Load(game_inputs_configs[i]);

		game_inputs[game_input.name] = game_input;
	}
}
