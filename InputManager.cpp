#include "InputManager.h"
#include "Module/ModuleInput.h"
#include "Main/Application.h"

InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

bool InputManager::GetGameInput(std::string &name)
{
	GameInput button = game_inputs[name];
	for(auto &key : button.keys)
	{
		if (App->input->GetKey(key))
			return true;
	}

	for(auto &mouse : button.mouse_buttons)
	{
		if (App->input->GetMouseButton(mouse))
			return true;
	}

	return false;
}

bool InputManager::GetGameInputDown(std::string & name)
{
	GameInput button = game_inputs[name];
	for (auto &key : button.keys)
	{
		if (App->input->GetKeyDown(key))
			return true;
	}

	for (auto &mouse : button.mouse_buttons)
	{
		if (App->input->GetMouseButtonDown(mouse))
			return true;
	}

	return false;
}

bool InputManager::GetGameInputUp(std::string & name)
{
	GameInput button = game_inputs[name];
	for (auto &key : button.keys)
	{
		if (App->input->GetKeyUp(key))
			return true;
	}

	for (auto &mouse : button.mouse_buttons)
	{
		if (App->input->GetMouseButtonUp(mouse))
			return true;
	}

	return false;
}
