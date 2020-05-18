#include "MainMenuLogic.h"

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
	buttons_transforms.push_back(&play_button->transform_2d);
	buttons_transforms.push_back(&help_button->transform_2d);
	buttons_transforms.push_back(&credits_button->transform_2d);
	buttons_transforms.push_back(&exit_button->transform_2d);

	buttons_components.push_back(static_cast<ComponentButton*>(play_button->GetComponent(Component::ComponentType::UI_BUTTON)));
	buttons_components.push_back(static_cast<ComponentButton*>(help_button->GetComponent(Component::ComponentType::UI_BUTTON)));
	buttons_components.push_back(static_cast<ComponentButton*>(credits_button->GetComponent(Component::ComponentType::UI_BUTTON)));
	buttons_components.push_back(static_cast<ComponentButton*>(exit_button->GetComponent(Component::ComponentType::UI_BUTTON)));

	audio_source = (ComponentAudioSource*) audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
	audio_source->PlayEvent("Play_ingame_music");
}


// Update is called once per frame
void MainMenuLogic::Update()
{
	for (unsigned int i = 0; i < buttons_components.size(); ++i)
	{
		if (buttons_components[i]->IsClicked())
		{
			OpenSubMenu(i);
		}
	}

	if (MenuController::ComfirmButtonPressed(*App->input))
	{
		OpenSubMenu(current_highlighted_button);
	}

	if(MenuController::ConfirmMovedUp(*App->input))
	{
		current_highlighted_button -= 1;
		current_highlighted_button = current_highlighted_button % 4;
		cursor->transform_2d.SetTranslation(float3(
				cursor->transform_2d.GetTranslation().x,
				buttons_transforms[current_highlighted_button]->GetTranslation().y,
				0.0f
		));
	}
	else if(MenuController::ConfirmMovedDown(*App->input))
	{
		current_highlighted_button += 1;
		current_highlighted_button = current_highlighted_button % 4;
		cursor->transform_2d.SetTranslation(float3(
			cursor->transform_2d.GetTranslation().x,
			buttons_transforms[current_highlighted_button]->GetTranslation().y,
			0.0f
		));
	}
}

void MainMenuLogic::OpenSubMenu(int menu_index)
{
	audio_source->PlayEvent("Click_fordward");
	//Change scene
	switch (menu_index)
	{
	case 0:
		character_selection_panel->SetEnabled(true);
		break;
	case 1:
		//Active help
		help_panel->SetEnabled(true);
		return;
	case 2:
		//Active credits
		credits_panel->SetEnabled(true);
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

	public_gameobjects.push_back(&character_selection_panel);
	variable_names.push_back(GET_VARIABLE_NAME(character_selection_panel));	
	
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



