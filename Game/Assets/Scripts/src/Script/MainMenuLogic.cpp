#include "MainMenuLogic.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentAudioSource.h"
#include "Filesystem/PathAtlas.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



MainMenuLogic* MainMenuLogicDLL()
{
	MainMenuLogic* instance = new MainMenuLogic();
	return instance;
}

MainMenuLogic::MainMenuLogic()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void MainMenuLogic::Awake()
{
	buttons.push_back(play_button);
	buttons.push_back(help_button);
	buttons.push_back(credits_button);
	buttons.push_back(exit_button);
	audio_source = (ComponentAudioSource*) audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
	audio_source->PlayEvent("Play_ingame_music");

	for (auto & child : credits_panel->children)
	{
		if (child->GetComponent(Component::ComponentType::UI_BUTTON))
		{
			credits_back_button = &child->transform_2d;
		}
	}
}

// Use this for initialization
void MainMenuLogic::Start()
{

}

// Update is called once per frame
void MainMenuLogic::Update()
{
	
	if(show_help && ComfirmButtonPressed())
	{
		owner->transform_2d.SetTranslation(float3(owner->transform_2d.GetTranslation().x, buttons[current]->transform_2d.GetTranslation().y, 0.0f));
		show_help = false;
		audio_source->PlayEvent("Click_backward");
		return;
	}

	if (show_credits && ComfirmButtonPressed())
	{
		credits_panel->SetEnabled(false);
		owner->transform_2d.SetTranslation(float3(owner->transform_2d.GetTranslation().x, buttons[current]->transform_2d.GetTranslation().y, 0.0f));
		show_credits = false;
		audio_source->PlayEvent("Click_backward");
		return;
	}

	if(show_credits || show_help)
	{
		return;
	}

	if (ComfirmButtonPressed())
	{
		audio_source->PlayEvent("Click_fordward");
		//Change scene
		switch (current)
		{
		case 0:
			App->scene->LoadScene("/Assets/Scenes/Production/MainScene.scene");
			break;
		case 1:
			//Active help
			help_panel->SetEnabled(true);
			show_help = true;
			return;
		case 2:
			//Active credits
			credits_panel->SetEnabled(true);
			show_credits = true;
			return;
		case 3:
			//Close game
			SDL_Event quit_event;
			quit_event.type = SDL_QUIT;
			SDL_PushEvent(&quit_event);
			break;
		default:
			break;
		}
	}

	if(ConfirmMovedUp())
	{
		current -= 1;
		current = current % 4;
		owner->transform_2d.SetTranslation(float3(owner->transform_2d.GetTranslation().x, buttons[current]->transform_2d.GetTranslation().y, 0.0f));
	}
	else if(ConfirmMovedDown())
	{
		current += 1;
		current = current % 4;

		owner->transform_2d.SetTranslation(float3(owner->transform_2d.GetTranslation().x,buttons[current]->transform_2d.GetTranslation().y,0.0f));
	}


}

// Use this for showing variables on inspector
void MainMenuLogic::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically
void MainMenuLogic::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&play_button);
	variable_names.push_back(GET_VARIABLE_NAME(play_button));

	public_gameobjects.push_back(&help_button);
	variable_names.push_back(GET_VARIABLE_NAME(help_button));

	public_gameobjects.push_back(&credits_button);
	variable_names.push_back(GET_VARIABLE_NAME(credits_button));

	public_gameobjects.push_back(&exit_button);
	variable_names.push_back(GET_VARIABLE_NAME(exit_button));

	public_gameobjects.push_back(&credits_panel);
	variable_names.push_back(GET_VARIABLE_NAME(credits_panel));

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

bool MainMenuLogic::ConfirmMovedUp()
{
	return (App->input->GetKeyDown(KeyCode::W) || App->input->GetControllerButtonDown(ControllerCode::UpDpad, ControllerID::ONE));
}

bool MainMenuLogic::ConfirmMovedDown()
{
	return (App->input->GetKeyDown(KeyCode::S) || App->input->GetControllerButtonDown(ControllerCode::DownDpad, ControllerID::ONE));
}

bool MainMenuLogic::ComfirmButtonPressed()
{
	return (App->input->GetKeyDown(KeyCode::Space) || App->input->GetControllerButtonDown(ControllerCode::A, ControllerID::ONE));
}


