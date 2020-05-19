#include "CreditsMenuController.h"

#include "Component/ComponentAudioSource.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "MainMenuController.h"
#include "UIMainMenuInputController.h"

#include "imgui.h"



CreditsMenuController* CreditsMenuControllerDLL()
{
	CreditsMenuController* instance = new CreditsMenuController();
	return instance;
}

CreditsMenuController::CreditsMenuController()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void CreditsMenuController::Awake()
{
	main_menu_controller = static_cast<MainMenuController*>(main_menu_game_object->GetComponentScript("MainMenuController")->script);
	audio_source = (ComponentAudioSource*)audio_controller->GetComponent(Component::ComponentType::AUDIO_SOURCE);
}

// Use this for initialization
void CreditsMenuController::Start()
{

}

// Update is called once per frame
void CreditsMenuController::Update()
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
	
	if (UIMainMenuInputController::ComfirmButtonPressed(*App->input))
	{
		Close();
	}
}

void CreditsMenuController::Open()
{
	enabled = true;
	just_opened = true;
	credits_panel->SetEnabled(true);
}

void CreditsMenuController::Close()
{
	enabled = false;
	credits_panel->SetEnabled(false);
	audio_source->PlayEvent("Click_backward");
	main_menu_controller->Open();
}

// Use this for showing variables on inspector
void CreditsMenuController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically 
void CreditsMenuController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&credits_panel);
	variable_names.push_back(GET_VARIABLE_NAME(credits_panel));

	public_gameobjects.push_back(&main_menu_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(main_menu_game_object));

	public_gameobjects.push_back(&audio_controller);
	variable_names.push_back(GET_VARIABLE_NAME(audio_controller));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void CreditsMenuController::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void CreditsMenuController::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }