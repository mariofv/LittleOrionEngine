#include "MainMenuController.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Filesystem/PathAtlas.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleUI.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "UIMainMenuInputController.h"
#include "CharacterSelectionMenuController.h"
#include "CreditsMenuController.h"
#include "HelpMenuController.h"

#include "imgui.h"



MainMenuController* MainMenuControllerDLL()
{
	MainMenuController* instance = new MainMenuController();
	return instance;
}

MainMenuController::MainMenuController()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void MainMenuController::Awake()
{
	buttons_transforms.push_back(&play_button->transform_2d);
	buttons_transforms.push_back(&help_button->transform_2d);
	buttons_transforms.push_back(&credits_button->transform_2d);
	buttons_transforms.push_back(&exit_button->transform_2d);

	buttons_components.push_back(static_cast<ComponentButton*>(play_button->GetComponent(Component::ComponentType::UI_BUTTON)));
	buttons_components.push_back(static_cast<ComponentButton*>(help_button->GetComponent(Component::ComponentType::UI_BUTTON)));
	buttons_components.push_back(static_cast<ComponentButton*>(credits_button->GetComponent(Component::ComponentType::UI_BUTTON)));
	buttons_components.push_back(static_cast<ComponentButton*>(exit_button->GetComponent(Component::ComponentType::UI_BUTTON)));

	character_selection_controller = static_cast<CharacterSelectionMenuController*>(character_selection_game_object->GetComponentScript("CharacterSelectionMenuController")->script);
	credits_controller = static_cast<CreditsMenuController*>(credits_game_object->GetComponentScript("CreditsMenuController")->script);
	help_controller = static_cast<HelpMenuController*>(help_game_object->GetComponentScript("HelpMenuController")->script);


	audio_source = (ComponentAudioSource*) audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
	audio_source->PlayEvent("Play_ingame_music");
}


// Update is called once per frame
void MainMenuController::Update()
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

	for (unsigned int i = 0; i < buttons_components.size(); ++i)
	{
		if (buttons_components[i]->IsClicked())
		{
			OpenSubMenu(i);
		}
	}

	if (UIMainMenuInputController::ComfirmButtonPressed(*App->input))
	{
		OpenSubMenu(current_highlighted_button);
	}

	if(UIMainMenuInputController::ConfirmMovedUp(*App->input))
	{
		current_highlighted_button -= 1;
		current_highlighted_button = current_highlighted_button % 4;
		cursor->transform_2d.SetTranslation(float3(
				cursor->transform_2d.GetTranslation().x,
				buttons_transforms[current_highlighted_button]->GetGlobalTranslation().y,
				0.0f
		));
	}
	else if(UIMainMenuInputController::ConfirmMovedDown(*App->input))
	{
		current_highlighted_button += 1;
		current_highlighted_button = current_highlighted_button % 4;
		cursor->transform_2d.SetTranslation(float3(
			cursor->transform_2d.GetTranslation().x,
			buttons_transforms[current_highlighted_button]->GetGlobalTranslation().y,
			0.0f
		));
	}
}

void MainMenuController::OpenSubMenu(int menu_index)
{
	audio_source->PlayEvent("Click_fordward");
	//Change scene
	switch (menu_index)
	{
	case 0:
		character_selection_controller->Open();
		break;
	case 1:
		//Active help
		help_controller->Open();
		break;
	case 2:
		//Active credits
		credits_controller->Open();
		break;
	case 3:
		//Close game
		SDL_Event quit_event;
		quit_event.type = SDL_QUIT;
		SDL_PushEvent(&quit_event);
		break;
	default:
		break;
	}

	Close();
}

void MainMenuController::Open()
{
	enabled = true;
	just_opened = true;
	main_menu_panel->SetEnabled(true);
}

void MainMenuController::Close()
{
	enabled = false;
	main_menu_panel->SetEnabled(false);
}

// Use this for showing variables on inspector
void MainMenuController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically
void MainMenuController::InitPublicGameObjects()
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

	public_gameobjects.push_back(&credits_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(credits_game_object));

	public_gameobjects.push_back(&help_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(help_game_object));

	public_gameobjects.push_back(&character_selection_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(character_selection_game_object));
	
	public_gameobjects.push_back(&main_menu_panel);
	variable_names.push_back(GET_VARIABLE_NAME(main_menu_panel));

	public_gameobjects.push_back(&cursor);
	variable_names.push_back(GET_VARIABLE_NAME(cursor));

	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}



