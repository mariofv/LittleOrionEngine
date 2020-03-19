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

	SDL_Init(SDL_INIT_GAMECONTROLLER);

	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			controller[i] = SDL_GameControllerOpen(i);
			//break;
		}
	}

	std::map<ControllerCode, KeyState> temp1;
	std::map<ControllerCode, KeyState> temp2;

	for (int i = 0; i < MAX_CONTROLLER_BUTTONS; ++i)
	{
		temp1[(ControllerCode)i] = KeyState::IDLE;
		temp2[(ControllerCode)i] = KeyState::IDLE;
	}

	controller_bible.push_back(temp1);
	controller_bible.push_back(temp2);

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
	left_joystick = { 0, 0 };
	right_joystick = { 0, 0 };
	left_controller_trigger = 0;
	right_controller_trigger = 0;
	left_joystick_raw = { 0, 0 };
	right_joystick_raw = { 0, 0 };
	left_controller_trigger_raw = 0;
	right_controller_trigger_raw = 0;

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

	for (int i = 0; i < controller_bible.size(); ++i)
	{
		for (auto& controller : controller_bible[i])
		{
			if (controller.second == KeyState::DOWN)
			{
				controller.second = KeyState::REPEAT;
			}
			else if (controller.second == KeyState::UP)
			{
				controller.second = KeyState::IDLE;
			}
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

		case SDL_CONTROLLERBUTTONDOWN:
			controller_bible[event.cbutton.which][(ControllerCode)event.cbutton.button] = KeyState::DOWN;
			break;

		case SDL_CONTROLLERBUTTONUP:
			controller_bible[event.cbutton.which][(ControllerCode)event.cbutton.button] = KeyState::UP;
			break;
		
		case SDL_CONTROLLERAXISMOTION: 
		{
			int which = event.cbutton.which;
			left_joystick = float2(SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_LEFTX), SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_LEFTY));
			right_joystick = float2(SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_RIGHTX), SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_RIGHTY));

			left_joystick_raw = Filter2D(SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_LEFTX), SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_LEFTY));
			right_joystick_raw = Filter2D(SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_RIGHTX), SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_RIGHTY));


			left_controller_trigger = SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_TRIGGERLEFT);
			left_controller_trigger_raw = left_controller_trigger / MAX_SDL_CONTROLLER_RANGE;
			right_controller_trigger = SDL_GameControllerGetAxis(controller[which], SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
			right_controller_trigger_raw = right_controller_trigger / MAX_SDL_CONTROLLER_RANGE;
		}
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
	SDL_GameControllerClose(controller[0]);
	SDL_GameControllerClose(controller[1]);
	return true;
}

// Returns true while the user holds down the key identified by name
ENGINE_API bool ModuleInput::GetKey(KeyCode key)
{
	return key_bible[key] == KeyState::REPEAT;
}

// Returns true during the frame the user starts pressing down the key identified by name
ENGINE_API bool ModuleInput::GetKeyDown(KeyCode key)
{
	return key_bible[key] == KeyState::DOWN;
}

// Returns true during the frame the user releases the key identified by name
ENGINE_API bool ModuleInput::GetKeyUp(KeyCode key)
{
	return key_bible[key] == KeyState::UP;
}

// Returns whether the given mouse button is held down
ENGINE_API bool ModuleInput::GetMouseButton(MouseButton button)
{
	return mouse_bible[button] == KeyState::REPEAT;
}

// Returns true during the frame the user pressed the given mouse button
ENGINE_API bool ModuleInput::GetMouseButtonDown(MouseButton button)
{
	return mouse_bible[button] == KeyState::DOWN;
}

// Returns true during the frame the user releases the given mouse button
ENGINE_API bool ModuleInput::GetMouseButtonUp(MouseButton button)
{
	return mouse_bible[button] == KeyState::UP;
}

ENGINE_API bool ModuleInput::GetControllerButton(ControllerCode code)
{
	return controller_bible[0][code] == KeyState::REPEAT;
}

ENGINE_API bool ModuleInput::GetControllerButtonDown(ControllerCode code)
{
	return controller_bible[0][code] == KeyState::DOWN;
}

ENGINE_API bool ModuleInput::GetControllerButtonUp(ControllerCode code)
{
	return controller_bible[0][code] == KeyState::UP;
}

ENGINE_API bool ModuleInput::GetControllerButton(ControllerCode code, int player_num)
{
	return controller_bible[player_num][code] == KeyState::REPEAT;
}

ENGINE_API bool ModuleInput::GetControllerButtonDown(ControllerCode code, int player_num)
{
	return controller_bible[player_num][code] == KeyState::DOWN;
}

ENGINE_API bool ModuleInput::GetControllerButtonUp(ControllerCode code, int player_num)
{
	return controller_bible[player_num][code] == KeyState::UP;
}

ENGINE_API bool ModuleInput::GetGameInput(const char* name)
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

ENGINE_API bool ModuleInput::GetGameInputDown(const char* name)
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

ENGINE_API bool ModuleInput::GetGameInputUp(const char* name)
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

ENGINE_API float2 ModuleInput::GetAxisContoller(ControllerAxis type) const
{
	switch (type)
	{
		case ControllerAxis::LEFT_JOYSTICK:
			return left_joystick;
			break;

		case ControllerAxis::RIGHT_JOYSTICK:
			return right_joystick;
			break;

		case ControllerAxis::LEFT_JOYSTICK_RAW:
			return left_joystick_raw;
			break;

		case ControllerAxis::RIGHT_JOYSTICK_RAW:
			return right_joystick_raw;
			break;
		default:
			break;
	}

	return float2(0.0f, 0.0f);
}

ENGINE_API Sint16 ModuleInput::GetTriggerController(ControllerAxis type) const
{
	switch (type)
	{
		case ControllerAxis::LEFT_TRIGGER:
			return left_controller_trigger;
			break;
		case ControllerAxis::RIGHT_TRIGGER:
			return right_controller_trigger;
			break;
		case ControllerAxis::LEFT_TRIGGER_RAW:
			return left_controller_trigger_raw;
			break;
		case ControllerAxis::RIGHT_TRIGGER_RAW:
			return right_controller_trigger_raw;
			break;

		default:
			break;
	}

	return 0;
}

ENGINE_API float2 ModuleInput::GetAxisContollerRaw(ControllerAxis type) const
{
	switch (type)
	{
		case ControllerAxis::LEFT_JOYSTICK_RAW:
			return left_joystick_raw;
			break;

		case ControllerAxis::RIGHT_JOYSTICK_RAW:
			return right_joystick_raw;
			break;
		default:
			break;
	}

	return float2(0.0f, 0.0f);
}

ENGINE_API float ModuleInput::GetTriggerControllerRaw(ControllerAxis type) const
{
	switch (type)
	{
	case ControllerAxis::LEFT_TRIGGER_RAW:
		return left_controller_trigger_raw;
		break;
	case ControllerAxis::RIGHT_TRIGGER_RAW:
		return right_controller_trigger_raw;
		break;

	default:
		break;
	}

	return 0.0f;
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

float2 ModuleInput::Filter2D(Sint16 input_x, Sint16 input_y) const
{

	const float dead_zone = 8000.0f;
	const float max_value = 30000.0f;

	float2 dir;
	dir.x = static_cast<float>(input_x);
	dir.y = static_cast<float>(input_y);

	float length = dir.Length();

	//if len < dead_zone then should be no input
	if(length < dead_zone)
	{
		dir = float2::zero;
	}
	else
	{
		//Compute fractional interpolation between dead zone and max_value
		float f = (length - dead_zone) / (max_value - dead_zone);

		//Clamp f between 0.0f and 1.0f
		f = math::Clamp(f, 0.0f, 1.0f);
		//Normalize the vector, and then scale it to the fractional value
		dir *= f / length;
	}


	return dir;
}
