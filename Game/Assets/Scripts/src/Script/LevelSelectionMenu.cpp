#include "LevelSelectionMenu.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentAudioSource.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "MenuController.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

LevelSelectionMenu* LevelSelectionMenuDLL()
{
	LevelSelectionMenu* instance = new LevelSelectionMenu();
	return instance;
}

LevelSelectionMenu::LevelSelectionMenu()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void LevelSelectionMenu::Awake()
{
	buttons.push_back(level1);
	buttons.push_back(level2);
	buttons.push_back(level3);
	audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
	awaked = true;

}

void LevelSelectionMenu::Update()
{
	if (!awaked)
	{
		Awake();
		return;
	}
	if (!enabled)
	{
		enabled = true;
		return;
	}

	if (!selecting_level && MenuController::ComfirmButtonPressed(*App->input))
	{
		//Back
		audio_source->PlayEvent("Click_backward");
		owner->parent->SetEnabled(false);
		previous_panel->SetEnabled(true);
		enabled = false;
		return;
	}
	if (selecting_level && MenuController::ComfirmButtonPressed(*App->input))
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

	if(MenuController::ConfirmMovedUp(*App->input) || MenuController::ConfirmMovedDown(*App->input))
	{
		selecting_level = !selecting_level;
		back_cursor->SetEnabled(!back_cursor->IsEnabled());
	}
	if (selecting_level && MenuController::ConfirmMovedLeft(*App->input))
	{
		current = (current - 1) == -1 ? buttons.size() - 1 : current - 1;
		current = current % buttons.size();
		owner->transform_2d.SetTranslation( buttons[current]->transform_2d.GetTranslation());
	}
	else if (selecting_level && MenuController::ConfirmMovedRight(*App->input))
	{
		current += 1;
		current = current % buttons.size();
		owner->transform_2d.SetTranslation(buttons[current]->transform_2d.GetTranslation());
	}

}

void LevelSelectionMenu::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

void LevelSelectionMenu::InitPublicGameObjects()
{

	public_gameobjects.push_back(&level1);
	variable_names.push_back(GET_VARIABLE_NAME(level1));

	public_gameobjects.push_back(&level2);
	variable_names.push_back(GET_VARIABLE_NAME(level2));

	public_gameobjects.push_back(&level3);
	variable_names.push_back(GET_VARIABLE_NAME(level3));

	public_gameobjects.push_back(&back);
	variable_names.push_back(GET_VARIABLE_NAME(back));

	public_gameobjects.push_back(&previous_panel);
	variable_names.push_back(GET_VARIABLE_NAME(previous_panel));

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



