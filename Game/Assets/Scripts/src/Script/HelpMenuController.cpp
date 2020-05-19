#include "HelpMenuController.h"
#include "Component/ComponentAudioSource.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"

#include "MainMenuController.h"
#include "UIMainMenuInputController.h"

#include "imgui.h"

HelpMenuController* HelpMenuControllerDLL()
{
	HelpMenuController* instance = new HelpMenuController();
	return instance;
}

HelpMenuController::HelpMenuController()
{
	panel = new PanelComponent();
}

void HelpMenuController::Awake()
{
	audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
	main_menu_controller = static_cast<MainMenuController*>(main_menu_panel->GetComponentScript("MainMenuController")->script);
}

void HelpMenuController::Update()
{
	if (!enabled)
	{
		return;
	}
	if (just_opened)
	{
		just_opened = false;
		return;
	}

	if (UIMainMenuInputController::ConfirmMovedLeft(*App->input) || UIMainMenuInputController::ConfirmMovedRight(*App->input))
	{
		help_joycon->SetEnabled(!help_joycon->IsEnabled());
		help_keyboard->SetEnabled(!help_keyboard->IsEnabled());
	}
	if (UIMainMenuInputController::ComfirmButtonPressed(*App->input))
	{
		Close();
		return;
	}
}

void HelpMenuController::Open()
{
	enabled = true;
	just_opened = true;
	help_menu_panel->SetEnabled(true);
	help_joycon->SetEnabled(true);
	help_keyboard->SetEnabled(false);
}

void HelpMenuController::Close()
{
	enabled = false;
	help_menu_panel->SetEnabled(false);
	audio_source->PlayEvent("Click_backward");
	main_menu_controller->Open();
}

void HelpMenuController::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void HelpMenuController::InitPublicGameObjects()
{
	public_gameobjects.push_back(&main_menu_panel);
	variable_names.push_back(GET_VARIABLE_NAME(main_menu_panel));

	public_gameobjects.push_back(&help_menu_panel);
	variable_names.push_back(GET_VARIABLE_NAME(help_menu_panel));

	public_gameobjects.push_back(&help_joycon);
	variable_names.push_back(GET_VARIABLE_NAME(help_joycon));

	public_gameobjects.push_back(&help_keyboard);
	variable_names.push_back(GET_VARIABLE_NAME(help_keyboard));


	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}


