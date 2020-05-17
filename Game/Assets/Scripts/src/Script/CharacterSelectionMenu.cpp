#include "CharacterSelectionMenu.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "Component/ComponentAudioSource.h"

#include "MenuController.h"

CharacterSelectionMenu::CharacterSelectionMenu()
{
	panel = new PanelComponent();
}

void CharacterSelectionMenu::Awake()
{
	buttons.push_back(back_button);
	buttons.push_back(level_selection_button);
	audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
	awaked = true;
}

void CharacterSelectionMenu::Update()
{
	if (!awaked)
	{
		Awake();
		return;
	}
	if (!enabled && owner->IsEnabled())
	{
		enabled = true;
		previous_panel->SetEnabled(false);
		return;
	}
	if (MenuController::ComfirmButtonPressed(*App->input))
	{
		audio_source->PlayEvent("Click_fordward");
		//Change scene
		switch (current)
		{
		case 0:
			//Back
			owner->parent->SetEnabled(false);
			previous_panel->SetEnabled(true);
			enabled = false;
			break;
		case 1:
			//Level selection
			owner->parent->SetEnabled(false);
			level_selection->SetEnabled(true);
			enabled = false;
			return;
		default:
			break;
		}
	}

	if (MenuController::ConfirmMovedUp(*App->input))
	{
		selecting_character = true;
	}
	if (MenuController::ConfirmMovedDown(*App->input))
	{
		selecting_character = false;
	}
	if (MenuController::ConfirmMovedLeft(*App->input))
	{
		if (!selecting_character)
		{
			current -= 1;
			current = current % buttons.size();
			ComponentTransform2D& button_transform = buttons[current]->transform_2d;
			float x = button_transform.GetTranslation().x - button_transform.GetWidth() / 2 - owner->transform_2d.GetWidth();
			owner->transform_2d.SetTranslation(float3(x, owner->transform_2d.GetTranslation().y, 0.0f));
		}
	}
	else if (MenuController::ConfirmMovedRight(*App->input))
	{
		if (!selecting_character)
		{
			current += 1;
			current = current % buttons.size();
			ComponentTransform2D& button_transform = buttons[current]->transform_2d;
			float x = button_transform.GetTranslation().x - button_transform.GetWidth() / 2 - owner->transform_2d.GetWidth();
			owner->transform_2d.SetTranslation(float3(x, owner->transform_2d.GetTranslation().y, 0.0f));
		}
	}

}

void CharacterSelectionMenu::OnInspector(ImGuiContext * context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void CharacterSelectionMenu::InitPublicGameObjects()
{
	public_gameobjects.push_back(&previous_panel);
	variable_names.push_back(GET_VARIABLE_NAME(previous_panel));


	public_gameobjects.push_back(&level_selection);
	variable_names.push_back(GET_VARIABLE_NAME(level_selection));

	public_gameobjects.push_back(&back_button);
	variable_names.push_back(GET_VARIABLE_NAME(back_button));

	public_gameobjects.push_back(&level_selection_button);
	variable_names.push_back(GET_VARIABLE_NAME(level_selection_button));

	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

CharacterSelectionMenu * CharacterSelectionMenuDLL()
{
	CharacterSelectionMenu* instance = new CharacterSelectionMenu();
	return instance;
}
