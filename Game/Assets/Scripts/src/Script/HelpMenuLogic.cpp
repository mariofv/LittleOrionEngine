#include "HelpMenuLogic.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"

#include "imgui.h"
HelpMenuLogic* HelpMenuLogicDLL()
{
	HelpMenuLogic* instance = new HelpMenuLogic();
	return instance;
}

HelpMenuLogic::HelpMenuLogic()
{
	panel = new PanelComponent();
}

void HelpMenuLogic::Update()
{
	if (ConfirmMovedLeft() || ConfirmMovedRight())
	{
		help_controller->SetEnabled(!help_controller->IsEnabled());
		help_keyboard->SetEnabled(!help_keyboard->IsEnabled());
	}
}

void HelpMenuLogic::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void HelpMenuLogic::InitPublicGameObjects()
{
	public_gameobjects.push_back(&help_controller);
	variable_names.push_back(GET_VARIABLE_NAME(help_controller));

	public_gameobjects.push_back(&help_keyboard);
	variable_names.push_back(GET_VARIABLE_NAME(help_keyboard));
	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

bool HelpMenuLogic::ConfirmMovedRight()
{
	return (App->input->GetKeyDown(KeyCode::D) || App->input->GetControllerButtonDown(ControllerCode::RightDpad, ControllerID::ONE));
}

bool HelpMenuLogic::ConfirmMovedLeft()
{
	return (App->input->GetKeyDown(KeyCode::A) || App->input->GetControllerButtonDown(ControllerCode::LeftDpad, ControllerID::ONE));
}

