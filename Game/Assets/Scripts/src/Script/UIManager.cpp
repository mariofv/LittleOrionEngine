#include "UIManager.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "DamageIndicatorSpawner.h"
#include "ProgressBar.h"

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

	damage_indicator_spawner = static_cast<DamageIndicatorSpawner*>(damage_indicator_spawner_game_object->GetComponentScript("DamageIndicatorSpawner")->script);;
}

// Use this for initialization
void UIManager::Start()
{

}

// Update is called once per frame
void UIManager::Update()
{
	if (transitioning && App->input->GetKeyDown(KeyCode::Space))
	{
		App->scene->LoadScene(0);
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

void UIManager::SpawnDamageIndicator(int damage, float3 position)
{
	damage_indicator_spawner->SpawnDamageIndicator(damage, position);
}

void UIManager::SetSecondPlayerUI()
{
	second_player_hud->SetEnabled(true);
}

void UIManager::SetWinScreen()
{
	win_screen->SetEnabled(true);
	transitioning = true;
}

void UIManager::SetLoseScreen()
{
	lose_screen->SetEnabled(true);
	transitioning = true;
}

// Use this for showing variables on inspector
void UIManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Checkbox("Transitioning", &transitioning);
}

//Use this for linking JUST GO automatically 
void UIManager::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&player1_progress_bar_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(player1_progress_bar_game_object));

	public_gameobjects.push_back(&player2_progress_bar_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(player2_progress_bar_game_object));

	public_gameobjects.push_back(&damage_indicator_spawner_game_object);
	variable_names.push_back(GET_VARIABLE_NAME(damage_indicator_spawner_game_object));

	public_gameobjects.push_back(&second_player_hud);
	variable_names.push_back(GET_VARIABLE_NAME(second_player_hud));

	public_gameobjects.push_back(&win_screen);
	variable_names.push_back(GET_VARIABLE_NAME(win_screen));

	public_gameobjects.push_back(&lose_screen);
	variable_names.push_back(GET_VARIABLE_NAME(lose_screen));

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