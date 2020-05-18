#include "HelpMenuLogic.h"
#include "Component/ComponentAudioSource.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"

#include "MenuController.h"

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

void HelpMenuLogic::Awake()
{
	audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
}

void HelpMenuLogic::Update()
{
	if (!enable && owner->IsEnabled())
	{
		enable = true;
		previous_panel->SetEnabled(false);
		help_keyboard->SetEnabled(false);
		return;
	}
	if (MenuController::ConfirmMovedLeft(*App->input) || MenuController::ConfirmMovedRight(*App->input))
	{
		help_joycon->SetEnabled(!help_joycon->IsEnabled());
		help_keyboard->SetEnabled(!help_keyboard->IsEnabled());
	}
	if (MenuController::ComfirmButtonPressed(*App->input))
	{
		enable = false;
		previous_panel->SetEnabled(true);
		owner->parent->SetEnabled(false);
		audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
		audio_source->PlayEvent("Click_backward");
		return;
	}
}

void HelpMenuLogic::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void HelpMenuLogic::InitPublicGameObjects()
{
	public_gameobjects.push_back(&help_joycon);
	variable_names.push_back(GET_VARIABLE_NAME(help_joycon));

	public_gameobjects.push_back(&help_keyboard);
	variable_names.push_back(GET_VARIABLE_NAME(help_keyboard));

	public_gameobjects.push_back(&previous_panel);
	variable_names.push_back(GET_VARIABLE_NAME(previous_panel));


	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}


