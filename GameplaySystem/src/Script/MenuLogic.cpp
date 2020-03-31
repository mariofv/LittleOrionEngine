#include "MenuLogic.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

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
	if(App->input->GetKeyDown(KeyCode::UpArrow))
	{
		current -= 1;
		current = current % 4;

		owner->transform_2d.position.y = buttons[current]->transform_2d.position.y;
	}
	else if(App->input->GetKeyDown(KeyCode::DownArrow))
	{
		current += 1;
		current = current % 4;

		owner->transform_2d.position.y = buttons[current]->transform_2d.position.y;

	}

	if(App->input->GetKeyDown(KeyCode::Space))
	{
		//Change scene
		App->editor->OpenScene(DEFAULT_SCENE_PATH);
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


	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
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

