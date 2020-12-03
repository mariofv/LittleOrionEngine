#include "ModuleInput.h"

#include "Component/ComponentCamera.h"

#include "EditorUI/Panel/PanelProjectExplorer.h"
#include "EditorUI/Panel/PanelScene.h"

#include "Filesystem/File.h"
#include "Filesystem/PathAtlas.h"
#include "Log/EngineLog.h"

#include "Main/Application.h"
#include "Module/ModuleWindow.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleScriptManager.h"
#include "Module/ModuleUI.h"

#include <Brofiler/Brofiler.h>
#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <SDL/SDL.h>


void GameInput::Load(Config &config)
{
	config.GetString("Name", name, "DefaultName");
	uint64_t size_keys = config.GetUInt("SizeKeys", 0);
	for (size_t i = 0; i < size_keys; ++i)
	{
		std::string name_k("k" + std::to_string(i));
		keys[i] = ((KeyCode)config.GetUInt(name_k, 0));
	}

	uint64_t size_mouse = config.GetUInt("SizeMouse", 0);
	for (size_t j = 0; j < size_mouse; ++j)
	{
		std::string name_m("m" + std::to_string(j));
		mouse_buttons[j] = ((MouseButton)config.GetUInt(name_m, 0));
	}

	uint64_t size_controller = config.GetUInt("SizeController", 0);
	for (size_t k = 0; k < size_controller; ++k)
	{
		std::string name_c("c" + std::to_string(k));
		controller_buttons[k] = ((ControllerCode)config.GetUInt(name_c, 0));
	}
}
// Called before render is available
bool ModuleInput::Init()
{
	APP_LOG_SECTION("************ Module Input Init ************");

	APP_LOG_INFO("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		APP_LOG_ERROR("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	for (size_t i = 0; i < MAX_KEYS; ++i)
	{
		key_bible[(KeyCode)i] = KeyState::IDLE;
	}

	for (size_t i = 0; i < MAX_MOUSE_BUTTONS; ++i)
	{
		mouse_bible[(MouseButton)i] = KeyState::IDLE;
	}

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

	APP_LOG_INFO("SDL input event system initialized correctly.");

	//Load Game Inputs
	game_inputs_file_path = App->filesystem->GetPath(RESOURCES_GAME_INPUTS_PATH + std::string("/") + RESOURCES_GAME_INPUTS_FILENAME);

	FileData game_inputs_data = game_inputs_file_path->GetFile()->Load();
	size_t readed_bytes = game_inputs_data.size;
	char* scene_file_data = (char*)game_inputs_data.buffer;

	if (scene_file_data != nullptr)
	{
		std::string serialized_scene_string = scene_file_data;
		delete[] scene_file_data;

		Config input_config(serialized_scene_string);
		LoadGameInputs(input_config);
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
	BROFILER_CATEGORY("Module Inputs PreUpdate", Profiler::Color::BlueViolet);

	mouse_motion = { 0, 0 };
	mouse_wheel_motion = 0;
	total_game_controllers = SDL_NumJoysticks();

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
	for(size_t x = 0; x < controller.size(); ++x)
	{
		for (auto& controller : controller[x]->controller_bible)
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
			{
				App->window->WindowResized(event.window.data1, event.window.data2);
			}
#if !GAME
			if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
			{
				App->scripts->CheckGameplayFolderStatus();
			}
#endif
			break;

		case SDL_MOUSEMOTION:
			mouse_position = float2(event.motion.x, event.motion.y);
			mouse_motion = float2(event.motion.xrel, event.motion.yrel);
			mouse_moving = event.motion.state;
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
			for (size_t i = 0; i < controller.size(); ++i)
			{
				if (event.cbutton.which == controller[i]->joystick)
				{
					controller[i]->controller_bible[(ControllerCode)event.cbutton.button] = KeyState::DOWN;
				}
			}
			break;

		case SDL_CONTROLLERBUTTONUP:
			for (size_t i = 0; i < controller.size(); ++i)
			{
				if (event.cbutton.which == controller[i]->joystick)
				{
					controller[i]->controller_bible[(ControllerCode)event.cbutton.button] = KeyState::UP;
				}
			}
			break;

		case SDL_CONTROLLERAXISMOTION:
		{
			for (size_t i = 0; i < controller.size(); ++i) 
			{
				if (event.caxis.which == controller[i]->joystick)
				{
					controller[i]->left_joystick_raw = float2(SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_LEFTX), SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_LEFTY));
					controller[i]->right_joystick_raw = float2(SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_RIGHTX), SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_RIGHTY));

					controller[i]->left_joystick= Filter2D(SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_LEFTX), SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_LEFTY));
					controller[i]->right_joystick = Filter2D(SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_RIGHTX), SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_RIGHTY));

					controller[i]->left_controller_trigger_raw = SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
					controller[i]->right_controller_trigger_raw= SDL_GameControllerGetAxis(controller[i]->controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

					controller[i]->left_controller_trigger = controller[i]->left_controller_trigger_raw / MAX_SDL_CONTROLLER_RANGE;
					controller[i]->right_controller_trigger = controller[i]->right_controller_trigger_raw / MAX_SDL_CONTROLLER_RANGE;
				}
			}

		}
		break;

		case SDL_DROPFILE:
		{
			char* dropped_filedir = event.drop.file;
			App->editor->project_explorer->ResourceDropFromOutside(dropped_filedir);
			SDL_free(dropped_filedir);
			break;
		}
		case SDL_CONTROLLERDEVICEADDED:
		{
			int which = event.cdevice.which;

			if (SDL_IsGameController(which))
			{
				AddGamepad(which);
			}
		}
		break;

		case SDL_CONTROLLERDEVICEREMOVED:
		{
			int which = event.cdevice.which;
			RemoveGamepad(which);
		}
		break;
		}
	}

	keys = SDL_GetKeyboardState(nullptr);

	for (size_t i = 0; i < MAX_KEYS; ++i)
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

	for(size_t i = 0; i < total_game_controllers; ++i)
	{
		SDL_GameControllerClose(controller[i]->controller);
	}

	controller.clear();
	return true;
}

// Returns true while the user holds down the key identified by name
bool ModuleInput::GetKey(KeyCode key)
{
	BROFILER_CATEGORY("Get Key", Profiler::Color::Lavender);
	//If map[x] does not find x it will add the default value
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

bool ModuleInput::GetControllerButton(ControllerCode code, ControllerID controller_id)
{
	return controller[(int)controller_id]->controller_bible[code] == KeyState::REPEAT;
}

bool ModuleInput::GetControllerButtonDown(ControllerCode code, ControllerID controller_id)
{
	return controller[(int)controller_id]->controller_bible[code] == KeyState::DOWN;
}

bool ModuleInput::GetControllerButtonUp(ControllerCode code, ControllerID controller_id)
{
	return controller[(int)controller_id]->controller_bible[code] == KeyState::UP;
}

bool ModuleInput::GetAnyKeyPressedDown() const
{
	for (auto& key : key_bible)
	{
		if (key.second == KeyState::DOWN)
		{
			return true;
		}
	}
	if(controller.size() > 0)
	{
		for (auto& contr : controller[0]->controller_bible)
		{
			if (contr.second == KeyState::DOWN)
			{
				return true;
			}
		}	
	}

	return false;
}

void ModuleInput::CreateGameInput(const GameInput& game_input)
{
	game_inputs[game_input.name] = game_input;

	Config config;
	SaveGameInputs(config);

	std::string serialized_game_input_string;
	config.GetSerializedString(serialized_game_input_string);

	Path* game_inputs_folder_path = App->filesystem->GetPath(RESOURCES_GAME_INPUTS_PATH);

	game_inputs_folder_path->Save(RESOURCES_GAME_INPUTS_FILENAME, serialized_game_input_string);
}

void ModuleInput::DeleteGameInput(const GameInput& game_input)
{
	game_inputs.erase(game_input.name);

	Config config;
	SaveGameInputs(config);

	std::string serialized_game_input_string;
	config.GetSerializedString(serialized_game_input_string);

	App->filesystem->Save(game_inputs_file_path->GetFullPath(), serialized_game_input_string);
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

float2 ModuleInput::GetAxisController(ControllerAxis type, ControllerID controller_id) const
{
	switch (type)
	{
		case ControllerAxis::LEFT_JOYSTICK:
			return controller[(int)controller_id]->left_joystick;

		case ControllerAxis::RIGHT_JOYSTICK:
			return controller[(int)controller_id]->right_joystick;

		case ControllerAxis::LEFT_JOYSTICK_RAW:
			return controller[(int)controller_id]->left_joystick_raw;

		case ControllerAxis::RIGHT_JOYSTICK_RAW:
			return controller[(int)controller_id]->right_joystick_raw;

		default:
			return float2(0.0f, 0.0f);
	}
}

float ModuleInput::GetTriggerController(ControllerAxis type, ControllerID controller_id) const
{
	switch (type)
	{
		case ControllerAxis::LEFT_TRIGGER:
			return controller[(int)controller_id]->left_controller_trigger;

		case ControllerAxis::RIGHT_TRIGGER:
			return controller[(int)controller_id]->right_controller_trigger;

		case ControllerAxis::LEFT_TRIGGER_RAW:
			return controller[(int)controller_id]->left_controller_trigger_raw;

		case ControllerAxis::RIGHT_TRIGGER_RAW:
			return controller[(int)controller_id]->right_controller_trigger_raw;

		default:
			return 0;
	}
}

float2 ModuleInput::GetAxisControllerRaw(ControllerAxis type, ControllerID controller_id) const
{
	switch (type)
	{
		case ControllerAxis::LEFT_JOYSTICK_RAW:
			return controller[(int)controller_id]->left_joystick_raw;

		case ControllerAxis::RIGHT_JOYSTICK_RAW:
			return controller[(int)controller_id]->right_joystick_raw;

		default:
			return float2(0.0f, 0.0f);
	}
}

Sint16 ModuleInput::GetTriggerControllerRaw(ControllerAxis type, ControllerID controller_id) const
{
	switch (type)
	{
		case ControllerAxis::LEFT_TRIGGER_RAW:
			return controller[(int)controller_id]->left_controller_trigger_raw;

		case ControllerAxis::RIGHT_TRIGGER_RAW:
			return controller[(int)controller_id]->right_controller_trigger_raw;

		default:
			return 0.0f;
	}
}

void ModuleInput::SaveGameInputs(Config& config)
{
	std::vector<Config> game_inputs_configs;

	for (auto& game_input : game_inputs)
	{
		Config game_inputs_config;
		game_input.second.Save(game_inputs_config);
		game_inputs_configs.push_back(game_inputs_config);
	}

	config.AddChildrenConfig(game_inputs_configs, "GameInputs");
}

void ModuleInput::LoadGameInputs(Config& serialized_config)
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
	dir.x = (float)input_x;
	dir.y = (float)input_y;

	float length = dir.Length();

	//if len < dead_zone then should be no input
	if (length < dead_zone)
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

bool ModuleInput::DetectedKeyboardInput(const GameInput& button, KeyState state)
{
	switch (state)
	{
	case KeyState::REPEAT:
	{
		for (auto& key : button.keys)
		{
			if (GetKey(key))
			{
				return true;
			}
		}

		for (auto& mouse : button.mouse_buttons)
		{
			if (GetMouseButton(mouse))
			{
				return true;
			}
		}
	}
	break;

	case KeyState::DOWN:
	{
		for (auto& key : button.keys)
		{
			if (GetKeyDown(key))
			{
				return true;
			}
		}

		for (auto& mouse : button.mouse_buttons)
		{
			if (GetMouseButtonDown(mouse))
			{
				return true;
			}
		}
	}
	break;

	case KeyState::UP:
	{
		for (auto& key : button.keys)
		{
			if (GetKeyUp(key))
			{
				return true;
			}
		}

		for (auto& mouse : button.mouse_buttons)
		{
			if (GetMouseButtonUp(mouse))
			{
				return true;
			}
		}
	}
	break;

	default:
		return false;
	}

	return false;
}

bool ModuleInput::DetectedGameControllerInput(const GameInput& button, KeyState state, ControllerID controller_id)
{
	switch (state)
	{
	case KeyState::REPEAT:
	{
		for (auto& controller : button.controller_buttons)
		{
			if (GetControllerButton(controller, controller_id))
			{
				return true;
			}
		}
	}
	break;

	case KeyState::DOWN:
	{
		for (auto& controller : button.controller_buttons)
		{
			if (GetControllerButtonDown(controller, controller_id))
			{
				return true;
			}
		}
	}
	break;

	case KeyState::UP:
	{
		for (auto& controller : button.controller_buttons)
		{
			if (GetControllerButtonUp(controller, controller_id))
			{
				return true;
			}
		}
	}
	break;

	default:
		return false;
	}

	return false;
}

void ModuleInput::AddGamepad(int device)
{
	Gamepad* gamepad = new Gamepad();
	gamepad->controller = SDL_GameControllerOpen(device);
	gamepad->device = device;
	gamepad->joystick = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gamepad->controller));
	for (size_t i = 0; i < MAX_CONTROLLER_BUTTONS; ++i)
	{
		gamepad->controller_bible[(ControllerCode)i] = KeyState::IDLE;
	}
	gamepad->left_joystick = float2(0, 0);
	gamepad->right_joystick = float2(0, 0);
	gamepad->left_controller_trigger = 0;
	gamepad->right_controller_trigger = 0;
	gamepad->left_joystick_raw = float2(0, 0);
	gamepad->right_joystick_raw = float2(0, 0);
	gamepad->left_controller_trigger_raw = 0;
	gamepad->right_controller_trigger_raw = 0;

	controller.emplace_back(gamepad);
	++total_game_controllers;
}

void ModuleInput::RemoveGamepad(int device)
{
	auto it = std::find_if(controller.begin(), controller.end(), [&device](Gamepad* pad)
	{
		return pad->device == device; 
	});
	if (it != controller.end())
	{
		--total_game_controllers;
		SDL_GameControllerClose(controller[device]->controller);
		controller.erase(it);
	}
}