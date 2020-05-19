#include "LevelSelectionMenuController.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentButton.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform2D.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"
#include "UIMainMenuInputController.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "CharacterSelectionMenuController.h"

#include "imgui.h"

LevelSelectionMenuController* LevelSelectionMenuControllerDLL()
{
	LevelSelectionMenuController* instance = new LevelSelectionMenuController();
	return instance;
}

LevelSelectionMenuController::LevelSelectionMenuController()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void LevelSelectionMenuController::Awake()
{
	cursor_positions.push_back(&level1_cursor_position->transform_2d);
	cursor_positions.push_back(&level2_cursor_position->transform_2d);
	cursor_positions.push_back(&level3_cursor_position->transform_2d);

	character_menu_controller = static_cast<CharacterSelectionMenuController*>(character_menu_game_object->GetComponentScript("CharacterSelectionMenuController")->script);
	back_button = static_cast<ComponentButton*>(back_button_game_object->GetComponent(Component::ComponentType::UI_BUTTON));
	audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
}

void LevelSelectionMenuController::Update()
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

	//Back
	if (!selecting_level && UIMainMenuInputController::ComfirmButtonPressed(*App->input))
	{
		Close();
		return;
	}
	if (!selecting_level && back_button->IsClicked())
	{
		Close();
		return;
	}

	if (selecting_level && UIMainMenuInputController::ComfirmButtonPressed(*App->input))
	{
		audio_source->PlayEvent("Click_fordward");
		//Change scene
		switch (current)
		{
		case 0:
			//level1
			App->scene->LoadScene(LEVEL1_BUILD_OPTIONS_POSITION);
			return;
		case 1:
			//level2
			return;
		case 2:
			//boss
			break;
		default:
			break;
		}
	}

	if(UIMainMenuInputController::ConfirmMovedUp(*App->input) || UIMainMenuInputController::ConfirmMovedDown(*App->input))
	{
		selecting_level = !selecting_level;
		back_cursor->SetEnabled(!back_cursor->IsEnabled());
	}

	float4 color = float4::one;
	if (selecting_level)
	{
		color.x = (math::Sin(App->time->time * 0.01f) + 1) * 0.5f;
	}
	static_cast<ComponentImage*>(level_selection_cursor->GetComponent(Component::ComponentType::UI_IMAGE))->SetColor(color);


	// This code is not needed atm as we have just one level
	/*
	if (selecting_level && UIMainMenuInputController::ConfirmMovedLeft(*App->input))
	{
		current = (current - 1) == -1 ? cursor_positions.size() - 1 : current - 1;
		current = current % cursor_positions.size();
		level_selection_cursor->transform_2d.SetTranslation(cursor_positions[current]->GetTranslation());
	}
	else if (selecting_level && UIMainMenuInputController::ConfirmMovedRight(*App->input))
	{
		current += 1;
		current = current % cursor_positions.size();
		level_selection_cursor->transform_2d.SetTranslation(cursor_positions[current]->GetTranslation());
	}
	*/
}

void LevelSelectionMenuController::Open()
{
	enabled = true;
	just_opened = true;
	selecting_level = true;
	level_selection_panel->SetEnabled(true);
	back_cursor->SetEnabled(false);
}

void LevelSelectionMenuController::Close()
{
	enabled = false;
	level_selection_panel->SetEnabled(false);
	character_menu_controller->Open();
	audio_source->PlayEvent("Click_backward");
}

void LevelSelectionMenuController::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void LevelSelectionMenuController::InitPublicGameObjects()
{

	public_gameobjects.push_back(&level1_cursor_position);
	variable_names.push_back(GET_VARIABLE_NAME(level1));

	public_gameobjects.push_back(&level2_cursor_position);
	variable_names.push_back(GET_VARIABLE_NAME(level2));

	public_gameobjects.push_back(&level3_cursor_position);
	variable_names.push_back(GET_VARIABLE_NAME(level3));

	public_gameobjects.push_back(&back_button_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(back));
	
	public_gameobjects.push_back(&character_menu_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(character_menu_game_object));

	public_gameobjects.push_back(&level_selection_panel);
	variable_names.push_back(GET_VARIABLE_NAME(level_selection_panel));

	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	public_gameobjects.push_back(&back_cursor);
	variable_names.push_back(GET_VARIABLE_NAME(back_cursor));	
	
	public_gameobjects.push_back(&level_selection_cursor);
	variable_names.push_back(GET_VARIABLE_NAME(level_selection_cursor));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}



