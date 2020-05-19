#include "LevelSelectionMenuController.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentAudioSource.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "UIMainMenuInputController.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "MainMenuController.h"

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
	buttons.push_back(level1);
	buttons.push_back(level2);
	buttons.push_back(level3);

	main_menu_controller = static_cast<MainMenuController*>(main_menu_game_object->GetComponentScript("MainMenuController")->script);
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

	if (!selecting_level && UIMainMenuInputController::ComfirmButtonPressed(*App->input))
	{
		//Back
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
			App->scene->LoadScene(LEVEL1_POSITION);
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
	if (selecting_level && UIMainMenuInputController::ConfirmMovedLeft(*App->input))
	{
		current = (current - 1) == -1 ? buttons.size() - 1 : current - 1;
		current = current % buttons.size();
		owner->transform_2d.SetTranslation( buttons[current]->transform_2d.GetTranslation());
	}
	else if (selecting_level && UIMainMenuInputController::ConfirmMovedRight(*App->input))
	{
		current += 1;
		current = current % buttons.size();
		owner->transform_2d.SetTranslation(buttons[current]->transform_2d.GetTranslation());
	}

}

void LevelSelectionMenuController::Open()
{
	enabled = true;
	just_opened = true;
	level_selection_panel->SetEnabled(true);
}

void LevelSelectionMenuController::Close()
{
	enabled = false;
	level_selection_panel->SetEnabled(false);
	main_menu_controller->Open();
	audio_source->PlayEvent("Click_backward");
}

void LevelSelectionMenuController::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void LevelSelectionMenuController::InitPublicGameObjects()
{

	public_gameobjects.push_back(&level1);
	variable_names.push_back(GET_VARIABLE_NAME(level1));

	public_gameobjects.push_back(&level2);
	variable_names.push_back(GET_VARIABLE_NAME(level2));

	public_gameobjects.push_back(&level3);
	variable_names.push_back(GET_VARIABLE_NAME(level3));

	public_gameobjects.push_back(&back);
	variable_names.push_back(GET_VARIABLE_NAME(back));
	
	public_gameobjects.push_back(&main_menu_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(main_menu_game_object));

	public_gameobjects.push_back(&level_selection_panel);
	variable_names.push_back(GET_VARIABLE_NAME(level_selection_panel));

	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	public_gameobjects.push_back(&back_cursor);
	variable_names.push_back(GET_VARIABLE_NAME(back_cursor));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}



