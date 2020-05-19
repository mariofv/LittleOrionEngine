#include "UIMainMenuInputController.h"

#include "Main/Application.h"
#include "Module/ModuleInput.h"


bool UIMainMenuInputController::ConfirmMovedUp( ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::W) || input.GetControllerButtonDown(ControllerCode::UpDpad, ControllerID::ONE));
}

bool UIMainMenuInputController::ConfirmMovedDown( ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::S) || input.GetControllerButtonDown(ControllerCode::DownDpad, ControllerID::ONE));
}

bool UIMainMenuInputController::ConfirmMovedRight( ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::D) || input.GetControllerButtonDown(ControllerCode::RightDpad, ControllerID::ONE));
}

bool UIMainMenuInputController::ConfirmMovedLeft( ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::A) || input.GetControllerButtonDown(ControllerCode::LeftDpad, ControllerID::ONE));
}

bool UIMainMenuInputController::ComfirmButtonPressed(ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::Space) || input.GetControllerButtonDown(ControllerCode::A, ControllerID::ONE));
}

bool UIMainMenuInputController::CancelButtonPressed(ModuleInput& input)
{
	return (input.GetKeyDown(KeyCode::Escape) || input.GetControllerButtonDown(ControllerCode::B, ControllerID::ONE));
}