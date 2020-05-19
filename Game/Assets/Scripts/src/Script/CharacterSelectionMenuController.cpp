#include "CharacterSelectionMenuController.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentButton.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"


#include "MainMenuController.h"
#include "LevelSelectionMenuController.h"
#include "UIMainMenuInputController.h"
#include "WorldManager.h"

CharacterSelectionMenuController::CharacterSelectionMenuController()
{
	panel = new PanelComponent();
}

void CharacterSelectionMenuController::Awake()
{
	main_menu_controller = static_cast<MainMenuController*>(main_menu_game_object->GetComponentScript("MainMenuController")->script);
	level_selection_controller = static_cast<LevelSelectionMenuController*>(level_selection_game_object->GetComponentScript("LevelSelectionMenuController")->script);

	world_manager = static_cast<WorldManager*>(world_manager_game_object->GetComponentScript("WorldManager")->script);

	back_button = static_cast<ComponentButton*>(back_button_game_object->GetComponent(Component::ComponentType::UI_BUTTON));
	cursor_transform = &cursor->transform_2d;

	audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
}

void CharacterSelectionMenuController::Update()
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

	if (!multiplayer && App->input->GetControllerButtonDown(ControllerCode::Start, ControllerID::ONE) || App->input->GetControllerButtonDown(ControllerCode::Start, ControllerID::TWO))
	{
		SwitchMultiplayer(true);
	}
	if (multiplayer &&  App->input->GetControllerButtonDown(ControllerCode::B, ControllerID::ONE) || App->input->GetControllerButtonDown(ControllerCode::B, ControllerID::TWO))
	{
		SwitchMultiplayer(false);
	}

	if (UIMainMenuInputController::ComfirmButtonPressed(*App->input))
	{
		if (!selecting_character)
		{
			Close();
		}
		else
		{
			audio_source->PlayEvent("Click_fordward");
			world_manager->singleplayer = !multiplayer;
			world_manager->player1_choice = player1_choice;
			level_selection_controller->Open();
			Close();
		}
	}

	if (back_button->IsClicked())
	{
		Close();
	}

	if (UIMainMenuInputController::ConfirmMovedUp(*App->input))
	{
		selecting_character = true;
		cursor->SetEnabled(false);
	}

	if (UIMainMenuInputController::ConfirmMovedDown(*App->input))
	{
		selecting_character = false;
		cursor->SetEnabled(true);
	}

	if (selecting_character && (UIMainMenuInputController::ConfirmMovedLeft(*App->input) || UIMainMenuInputController::ConfirmMovedRight(*App->input)))
	{
		SelectCharacter();
	}

}

void CharacterSelectionMenuController::SelectCharacter()
{
	player1_choice = !player1_choice;
	const float3& translation = player1_choice ? female_character_position->transform_2d.GetTranslation() : male_character_position->transform_2d.GetTranslation();
	character_selector1->transform_2d.SetTranslation(translation);
	if (multiplayer)
	{
		const float3& translation_p2 = player1_choice ? male_character_position->transform_2d.GetTranslation() : female_character_position->transform_2d.GetTranslation();
		character_selector2->transform_2d.SetTranslation(translation_p2);
	}
}

void CharacterSelectionMenuController::SwitchMultiplayer(bool enabled)
{
	multiplayer = enabled;
	if (multiplayer)
	{
		App->input->singleplayer_input = true;
		character_selector2->SetEnabled(true);
		const float3& translation_p2 = player1_choice ? male_character_position->transform_2d.GetTranslation() : female_character_position->transform_2d.GetTranslation();
		character_selector2->transform_2d.SetTranslation(translation_p2);
	}
	else
	{
		App->input->singleplayer_input = false;
		character_selector2->SetEnabled(false);
	}
}

void CharacterSelectionMenuController::Open()
{
	enabled = true;
	just_opened = true;
	character_selection_panel->SetEnabled(true);
	selecting_character = false;
	SwitchMultiplayer(false);
}

void CharacterSelectionMenuController::Close()
{
	enabled = false;
	character_selection_panel->SetEnabled(false);
	main_menu_controller->Open();
	audio_source->PlayEvent("Click_backward");
}

void CharacterSelectionMenuController::OnInspector(ImGuiContext * context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void CharacterSelectionMenuController::InitPublicGameObjects()
{
	public_gameobjects.push_back(&character_selection_panel);
	variable_names.push_back(GET_VARIABLE_NAME(character_selection_panel));

	public_gameobjects.push_back(&main_menu_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(main_menu_game_object));

	public_gameobjects.push_back(&level_selection_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(level_selection_game_object));

	public_gameobjects.push_back(&back_button_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(back_button_game_object));

	public_gameobjects.push_back(&world_manager_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(world_manager_game_object));

	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	public_gameobjects.push_back(&male_character_position);
	variable_names.push_back(GET_VARIABLE_NAME(male_character_position));

	public_gameobjects.push_back(&female_character_position);
	variable_names.push_back(GET_VARIABLE_NAME(female_character_position));


	public_gameobjects.push_back(&character_selector1);
	variable_names.push_back(GET_VARIABLE_NAME(character_selector1));

	public_gameobjects.push_back(&character_selector2);
	variable_names.push_back(GET_VARIABLE_NAME(character_selector2));

	public_gameobjects.push_back(&cursor);
	variable_names.push_back(GET_VARIABLE_NAME(cursor));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

CharacterSelectionMenuController * CharacterSelectionMenuControllerDLL()
{
	CharacterSelectionMenuController* instance = new CharacterSelectionMenuController();
	return instance;
}
