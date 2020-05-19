#include "CharacterSelectionMenuController.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentImage.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"


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

	switch (character_selection_status)
	{
		case CharacterSelectionStatus::BACK_HOVERED:

			if (UIMainMenuInputController::ConfirmMovedUp(*App->input))
			{
				character_selection_status = CharacterSelectionStatus::SELECTING_PLAYER;
				cursor->SetEnabled(false);
			}

			if (UIMainMenuInputController::ComfirmButtonPressed(*App->input))
			{
				Close();
			}
			break;

		case CharacterSelectionStatus::SELECTING_PLAYER:

			if (UIMainMenuInputController::ConfirmMovedDown(*App->input))
			{
				character_selection_status = CharacterSelectionStatus::BACK_HOVERED;
				cursor->SetEnabled(true);
			}

			if (UIMainMenuInputController::ComfirmButtonPressed(*App->input))
			{
					audio_source->PlayEvent("Click_fordward");
					character_selection_status = CharacterSelectionStatus::PLAYER_SELECTED;
			}

			if (UIMainMenuInputController::ConfirmMovedLeft(*App->input) || UIMainMenuInputController::ConfirmMovedRight(*App->input))
			{
				SwitchCharacterSelection();
			}
			break;

		case CharacterSelectionStatus::PLAYER_SELECTED:

			if (UIMainMenuInputController::ComfirmButtonPressed(*App->input))
			{
				audio_source->PlayEvent("Click_fordward");
				world_manager->singleplayer = !multiplayer;
				world_manager->player1_choice = player1_choice;
				level_selection_controller->Open();
				Close();
			}

			if (UIMainMenuInputController::CancelButtonPressed(*App->input))
			{
				audio_source->PlayEvent("Click_backward");
				character_selection_status = CharacterSelectionStatus::SELECTING_PLAYER;
			}
			break;
	}

	if (back_button->IsClicked())
	{
		Close();
	}

	UpdateCursorsColors();
}

void CharacterSelectionMenuController::SwitchCharacterSelection()
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

void CharacterSelectionMenuController::UpdateCursorsColors()
{
	float4 color = float4::one;
	switch (character_selection_status)
	{
		case CharacterSelectionStatus::BACK_HOVERED:
			color.x = 1.f;
			break;

		case CharacterSelectionStatus::SELECTING_PLAYER:
			color.x = (math::Sin(App->time->time * 0.01f) + 1) * 0.5f;
			break;

		case CharacterSelectionStatus::PLAYER_SELECTED:
			color.x = 0;
			break;
	}

	static_cast<ComponentImage*>(character_selector1->GetComponent(Component::ComponentType::UI_IMAGE))->SetColor(color);
	if (multiplayer)
	{
		static_cast<ComponentImage*>(character_selector2->GetComponent(Component::ComponentType::UI_IMAGE))->SetColor(color);
	}
}

void CharacterSelectionMenuController::Open()
{
	enabled = true;
	just_opened = true;
	character_selection_panel->SetEnabled(true);
	character_selection_status = CharacterSelectionStatus::SELECTING_PLAYER;
	cursor->SetEnabled(false);
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
