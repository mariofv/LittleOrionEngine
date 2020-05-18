#include "UIManager.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"


UIManager* UIManagerDLL()
{
	UIManager* instance = new UIManager();
	return instance;
}

UIManager::UIManager()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void UIManager::Awake()
{
	player1_progress_bar = static_cast<ProgressBar*>(player1_progress_bar_game_object->GetComponentScript("ProgressBar")->script);
	player2_progress_bar = static_cast<ProgressBar*>(player2_progress_bar_game_object->GetComponentScript("ProgressBar")->script);
}

// Use this for initialization
void UIManager::Start()
{

}

// Update is called once per frame
void UIManager::Update()
{
	if (App->input->GetKeyDown(KeyCode::Space))
	{
		SetPlayer1Health(0.1f);
	}
}

void UIManager::SetPlayer1Health(float percentage)
{
	player1_progress_bar->SetProgress(percentage);
}

void UIManager::SetPlayer2Health(float percentage)
{
	player2_progress_bar->SetProgress(percentage);
}

// Use this for showing variables on inspector
void UIManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

}

//Use this for linking JUST GO automatically 
void UIManager::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&player1_progress_bar_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(player1_progress_bar_game_object));

	public_gameobjects.push_back(&player2_progress_bar_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(player2_progress_bar_game_object));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void UIManager::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void UIManager::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }