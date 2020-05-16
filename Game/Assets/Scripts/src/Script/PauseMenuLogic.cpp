#include "PauseMenuLogic.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleScene.h"

#include "Component/ComponentAudioSource.h"

#include "MenuController.h"

#include "imgui.h"
PauseMenuLogic* PauseMenuLogicDLL()
{
	PauseMenuLogic* instance = new PauseMenuLogic();
	return instance;
}

PauseMenuLogic::PauseMenuLogic()
{
	panel = new PanelComponent();
}

void PauseMenuLogic::Awake()
{
	buttons.push_back(main_menu_button);
	buttons.push_back(help_button);
	buttons.push_back(resume_button);
	//buttons.push_back(level_selection_button);
	audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
	current = buttons.size() - 1;

}

void PauseMenuLogic::Update()
{
	if (!game_paused && owner->IsEnabled())
	{
		game_paused = true;
		time_scale = App->time->time_scale;
		//App->time->time_scale = 0.0f;
		audio_source->PlayEvent("Play_ingame_music");
	}
	if (MenuController::ComfirmButtonPressed(*App->input))
	{
		audio_source->PlayEvent("Click_fordward");
		//Change scene
		switch (current)
		{
		case 0:
			App->scene->LoadScene(MAIN_MENU_SCENE);
			break;
		case 1:
			//Active help
			help_panel->SetEnabled(true);
			return;
		case 2:
			//Resume game
			game_paused = false;
			App->time->time_scale = time_scale;
			owner->parent->SetEnabled(false);
			audio_source->StopEvent("Play_ingame_music");
			return;
		case 3:
			//Active level selection
			//load
			break;
		default:
			break;
		}
	}
	if (MenuController::ConfirmMovedUp(*App->input))
	{
		current = (current - 1) == -1 ? buttons.size() -1: current-1;
		owner->transform_2d.SetTranslation(float3(owner->transform_2d.GetTranslation().x, buttons[current]->transform_2d.GetTranslation().y, 0.0f));
	}

	else if (MenuController::ConfirmMovedDown(*App->input))
	{
		current += 1;
		current = current % buttons.size();
		owner->transform_2d.SetTranslation(float3(owner->transform_2d.GetTranslation().x, buttons[current]->transform_2d.GetTranslation().y, 0.0f));
	}
}

void PauseMenuLogic::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void PauseMenuLogic::InitPublicGameObjects()
{
	public_gameobjects.push_back(&resume_button);
	variable_names.push_back(GET_VARIABLE_NAME(resume_button));

	public_gameobjects.push_back(&help_button);
	variable_names.push_back(GET_VARIABLE_NAME(help_button));

	/*public_gameobjects.push_back(&level_selection_button);
	variable_names.push_back(GET_VARIABLE_NAME(level_selection_button));*/

	public_gameobjects.push_back(&main_menu_button);
	variable_names.push_back(GET_VARIABLE_NAME(main_menu_button));

	public_gameobjects.push_back(&help_panel);
	variable_names.push_back(GET_VARIABLE_NAME(help_panel));

	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}


