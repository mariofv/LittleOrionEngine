#include "MenuController.h"

#include "Main/Application.h"
#include "Module/ModuleInput.h"


bool MenuController::ConfirmMovedUp( ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::W) || input.GetControllerButtonDown(ControllerCode::UpDpad, ControllerID::ONE));
}

bool MenuController::ConfirmMovedDown( ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::S) || input.GetControllerButtonDown(ControllerCode::DownDpad, ControllerID::ONE));
}

bool MenuController::ComfirmButtonPressed( ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::Space) || input.GetControllerButtonDown(ControllerCode::A, ControllerID::ONE));
}


bool MenuController::ConfirmMovedRight( ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::D) || input.GetControllerButtonDown(ControllerCode::RightDpad, ControllerID::ONE));
}

bool MenuController::ConfirmMovedLeft( ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::A) || input.GetControllerButtonDown(ControllerCode::LeftDpad, ControllerID::ONE));
}