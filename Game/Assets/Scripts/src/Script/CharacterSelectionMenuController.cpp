#include "CharacterSelectionMenuController.h"

#include "Component/ComponentAudioSource.h"

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
	buttons.push_back(back_button);
	buttons.push_back(level_selection_button);

	main_menu_controller = static_cast<MainMenuController*>(main_menu_game_object->GetComponentScript("MainMenuController")->script);
	level_selection_controller = static_cast<LevelSelectionMenuController*>(level_selection_game_object->GetComponentScript("LevelSelectionMenuController")->script);

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

	if (selecting_character  && UIMainMenuInputController::ComfirmButtonPressed(*App->input))
	{
		App->input->singleplayer_input = !App->input->singleplayer_input;
		character_selector2->SetEnabled(!character_selector2->IsEnabled());
		const float3& translation_p2 = player1_choice ? p2_position->transform_2d.GetTranslation() : p1_position->transform_2d.GetTranslation();
		character_selector2->transform_2d.SetTranslation(translation_p2);

		confirm_multiplayer->SetEnabled(!confirm_multiplayer->IsEnabled());
		confirm_singleplayer->SetEnabled(!confirm_singleplayer->IsEnabled());
	}

	if (!selecting_character && UIMainMenuInputController::ComfirmButtonPressed(*App->input))
	{
		OpenSubMenu(current);
	}

	if (UIMainMenuInputController::ConfirmMovedUp(*App->input))
	{
		selecting_character = true;
		confirm_multiplayer->SetEnabled(App->input->singleplayer_input);
		confirm_singleplayer->SetEnabled(!App->input->singleplayer_input);
	}
	if (UIMainMenuInputController::ConfirmMovedDown(*App->input))
	{
		selecting_character = false;
		confirm_singleplayer->SetEnabled(false);
		confirm_multiplayer->SetEnabled(false);
	}
	if (UIMainMenuInputController::ConfirmMovedLeft(*App->input))
	{
		if (!selecting_character)
		{
			current -= 1;
			current = current % buttons.size();
			ComponentTransform2D& button_transform = buttons[current]->transform_2d;
			float x = button_transform.GetTranslation().x - button_transform.GetWidth() / 2 - owner->transform_2d.GetWidth();
			owner->transform_2d.SetTranslation(float3(x, owner->transform_2d.GetTranslation().y, 0.0f));
		}
		else
		{
			SelectCharacter();
		}
	
	}
	else if (UIMainMenuInputController::ConfirmMovedRight(*App->input))
	{	
		if (!selecting_character)
		{
			current += 1;
			current = current % buttons.size();
			ComponentTransform2D& button_transform = buttons[current]->transform_2d;
			float x = button_transform.GetTranslation().x - button_transform.GetWidth() / 2 - owner->transform_2d.GetWidth();
			owner->transform_2d.SetTranslation(float3(x, owner->transform_2d.GetTranslation().y, 0.0f));
		}
		else
		{
			SelectCharacter();
		}
	}

}

void CharacterSelectionMenuController::SelectCharacter()
{
	player1_choice = !player1_choice;
	const float3& translation = player1_choice ? p1_position->transform_2d.GetTranslation() : p2_position->transform_2d.GetTranslation();
	character_selector1->transform_2d.SetTranslation(translation);
	if (!App->input->singleplayer_input)
	{
		const float3& translation_p2 = player1_choice ? p2_position->transform_2d.GetTranslation() : p1_position->transform_2d.GetTranslation();
		character_selector2->transform_2d.SetTranslation(translation_p2);
	}
}

void CharacterSelectionMenuController::OpenSubMenu(int index)
{
	//Change scene
	switch (index)
	{
	case 0:
		//Back
		Close();
		break;
	case 1:
		//Level selection
		audio_source->PlayEvent("Click_fordward");
		level_selection_controller->Open();
		Close();
		break;
	default:
		break;
	}
	return;
}

void CharacterSelectionMenuController::Open()
{
	enabled = true;
	just_opened = true;
	character_selection_panel->SetEnabled(true);
	if (App->input->singleplayer_input)
	{
		character_selector2->SetEnabled(false);
	}
	confirm_multiplayer->SetEnabled(false);
	confirm_singleplayer->SetEnabled(false);
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

	public_gameobjects.push_back(&back_button);
	variable_names.push_back(GET_VARIABLE_NAME(back_button));

	public_gameobjects.push_back(&level_selection_button);
	variable_names.push_back(GET_VARIABLE_NAME(level_selection_button));

	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	public_gameobjects.push_back(&p1_position);
	variable_names.push_back(GET_VARIABLE_NAME(p1_position));

	public_gameobjects.push_back(&p2_position);
	variable_names.push_back(GET_VARIABLE_NAME(p2_position));


	public_gameobjects.push_back(&character_selector1);
	variable_names.push_back(GET_VARIABLE_NAME(character_selector1));

	public_gameobjects.push_back(&character_selector2);
	variable_names.push_back(GET_VARIABLE_NAME(character_selector2));

	public_gameobjects.push_back(&confirm_multiplayer);
	variable_names.push_back(GET_VARIABLE_NAME(confirm_multiplayer));


	public_gameobjects.push_back(&confirm_singleplayer);
	variable_names.push_back(GET_VARIABLE_NAME(confirm_singleplayer));

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
