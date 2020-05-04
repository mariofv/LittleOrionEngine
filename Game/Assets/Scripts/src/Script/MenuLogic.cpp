#include "MenuLogic.h"

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



MenuLogic* MenuLogicDLL()
{
	MenuLogic* instance = new MenuLogic();
	return instance;
}

MenuLogic::MenuLogic()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void MenuLogic::Awake()
{
	buttons.push_back(button0);
	buttons.push_back(button1);
	buttons.push_back(button2);
	buttons.push_back(button3);

}

// Use this for initialization
void MenuLogic::Start()
{

}

// Update is called once per frame
void MenuLogic::Update()
{

	if(show_help && ComfirmButtonPressed())
	{
		help_controller->SetEnabled(false);
		help_keyboard->SetEnabled(false);
		owner->transform_2d.SetPosition(&float3(owner->transform_2d.position.x, buttons[current]->transform_2d.position.y, owner->transform_2d.position.z));
		show_help = false;
		return;
	}

	if (show_credits && ComfirmButtonPressed())
	{
		credits->SetEnabled(false);
		owner->transform_2d.SetPosition(&float3(owner->transform_2d.position.x, buttons[current]->transform_2d.position.y, owner->transform_2d.position.z));
		show_credits = false;
		return;
	}

	if(show_help && (App->input->GetKeyDown(KeyCode::D) || App->input->GetControllerButtonDown(ControllerCode::RightDpad, ControllerID::ONE) || 
		App->input->GetKeyDown(KeyCode::A) || App->input->GetControllerButtonDown(ControllerCode::LeftDpad, ControllerID::ONE)))
	{
		help_controller->SetEnabled(!help_controller->IsEnabled());
		help_keyboard->SetEnabled(!help_keyboard->IsEnabled());
		App->ui->SortComponentsUI();
	}

	if(show_credits || show_help)
	{
		return;
	}

	if (ComfirmButtonPressed())
	{
		//Change scene
		switch (current)
		{
		case 0:
			App->scene->LoadScene("/Assets/Scenes/Production/MainScene.scene");
			break;
		case 1:
			//Active help
			help_controller->SetEnabled(true);
			help_keyboard->SetEnabled(false);
			owner->transform_2d.SetPosition(&float3(owner->transform_2d.position.x, (help_controller->transform_2d.height * -236.0f) / 604.0f, owner->transform_2d.position.z));
			show_help = true;
			return;
		case 2:
			//Active credits
			credits->SetEnabled(true);
			owner->transform_2d.SetPosition(&float3(owner->transform_2d.position.x, (credits->transform_2d.height * -236.0f) / 604.0f, owner->transform_2d.position.z));
			show_credits = true;
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

	if(App->input->GetKeyDown(KeyCode::W) || App->input->GetControllerButtonDown(ControllerCode::UpDpad, ControllerID::ONE))
	{
		current -= 1;
		current = current % 4;

		owner->transform_2d.SetPosition(&float3(owner->transform_2d.position.x, buttons[current]->transform_2d.position.y, owner->transform_2d.position.z));
	}
	else if(App->input->GetKeyDown(KeyCode::S) || App->input->GetControllerButtonDown(ControllerCode::DownDpad, ControllerID::ONE))
	{
		current += 1;
		current = current % 4;

		owner->transform_2d.SetPosition(&float3(owner->transform_2d.position.x, buttons[current]->transform_2d.position.y, owner->transform_2d.position.z));
	}

}

// Use this for showing variables on inspector
void MenuLogic::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically 
void MenuLogic::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&button0);
	variable_names.push_back(GET_VARIABLE_NAME(button0));

	public_gameobjects.push_back(&button1);
	variable_names.push_back(GET_VARIABLE_NAME(button1));

	public_gameobjects.push_back(&button2);
	variable_names.push_back(GET_VARIABLE_NAME(button2));

	public_gameobjects.push_back(&button3);
	variable_names.push_back(GET_VARIABLE_NAME(button3));

	public_gameobjects.push_back(&help_controller);
	variable_names.push_back(GET_VARIABLE_NAME(help_controller));

	public_gameobjects.push_back(&help_keyboard);
	variable_names.push_back(GET_VARIABLE_NAME(help_keyboard));
	
	public_gameobjects.push_back(&credits);
	variable_names.push_back(GET_VARIABLE_NAME(credits));


	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
bool MenuLogic::ComfirmButtonPressed()
{
	return (App->input->GetKeyDown(KeyCode::Space) || App->input->GetControllerButtonDown(ControllerCode::A, ControllerID::ONE));
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void MenuLogic::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void MenuLogic::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }

