#include "WorldManager.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Filesystem/PathAtlas.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "CameraController.h"
#include "PlayerController.h"
#include "UIManager.h"


bool WorldManager::singleplayer;
bool WorldManager::player1_choice;
bool WorldManager::player2_choice;


WorldManager* WorldManagerDLL()
{
	WorldManager* instance = new WorldManager();
	return instance;
}

WorldManager::WorldManager()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void WorldManager::Awake()
{
	if(on_main_menu)
	{
		return;
	}

	////Male model
	player1_go = App->scene->GetGameObjectByName("Player1");
	ComponentScript* player1_controller_component = (ComponentScript*)player1_go->GetComponentScript("PlayerController");
	player1_controller = static_cast<PlayerController*>(player1_controller_component->script);
	
	////Female model
	player2_go = App->scene->GetGameObjectByName("Player2");
	ComponentScript* player2_controller_component = (ComponentScript*)player2_go->GetComponentScript("PlayerController");
	player2_controller = static_cast<PlayerController*>(player2_controller_component->script);

	GameObject* hole_go = App->scene->GetGameObjectByName("Mesh collider HOLE_0");
	hole = static_cast<ComponentCollider*>(hole_go->GetComponent(Component::ComponentType::COLLIDER));
	
	//Logic of choosing character and single/multi player
	//Singleplayer
	if(singleplayer)
	{
		//If player1_choice == 0 he is chosing male model
		if(!player1_choice)
		{
			player1_controller->player = 1;
			player2_controller->player = 2;
			player2_controller->is_alive = false;
			player2_go->SetEnabled(false);
			player2_go->transform.SetTranslation(float3(100.f, 100.f, 100.f));
		}
		//Chosing female model
		else
		{
			player2_controller->player = 1;
			player1_controller->player = 2;
			player1_controller->is_alive = false;
			player1_go->SetEnabled(false);
			player1_go->transform.SetTranslation(float3(100.f, 100.f, 100.f));
		}
	}
	//Multiplayer
	else
	{
		//If player1_choice == 0 he is chosing male model
		if (!player1_choice)
		{
			player1_controller->player = 1;
			player2_controller->player = 2;
		}
		//Chosing female model
		else
		{
			player2_controller->player = 1;
			player1_controller->player = 2;
		}
	}

	GameObject* event_manager_go = App->scene->GetGameObjectByName("EventManager");
	ComponentScript* event_manager_component = event_manager_go->GetComponentScript("EventManager");
	event_manager = static_cast<EventManager*>(event_manager_component->script);

	GameObject* ui_manager_go = App->scene->GetGameObjectByName("UIManager");
	ComponentScript* ui_manager_component = ui_manager_go->GetComponentScript("UIManager");
	ui_manager = static_cast<UIManager*>(ui_manager_component->script);

	GameObject* camera_manager_go = App->scene->GetGameObjectByName("Main Camera");
	ComponentScript* camera_manager_component = camera_manager_go->GetComponentScript("CameraController");
	camera_manager = static_cast<CameraController*>(camera_manager_component->script);

	if(!singleplayer)
	{
		ui_manager->SetSecondPlayerUI();
	}

	InitTriggers();
}

// Use this for initialization
void WorldManager::Start()
{

}

// Update is called once per frame
void WorldManager::Update()
{
	if (on_main_menu)
	{
		return;
	}
	if(!disable_hole)
	{
		CheckHole();
	}
	CheckTriggers();
	/*
	if(health_component->percentage <= 0.0f)
	{
		//Lose
		player_controller->Disable();
		lose_component->Enable();
		transition = true;
	}

	if(OnTriggerEnter())
	{
		//Win
		player_controller->Disable();
		win_component->Enable();
		transition = true;
	}

	if(transition && App->input->GetAnyKeyPressedDown())
	{
		App->scene->LoadScene(0);
	}
	*/

	if(event_manager->current_event_beated == 2)
	{
		//We won the level!
		player1_controller->owner->SetEnabled(false);
		player2_controller->owner->SetEnabled(false);
		ui_manager->SetWinScreen();
	}


	if(CheckLose())
	{
		player1_controller->owner->SetEnabled(false);
		player2_controller->owner->SetEnabled(false);
		ui_manager->SetLoseScreen();
	}
}

// Use this for showing variables on inspector
void WorldManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Checkbox("Singleplayer", &singleplayer);
	ImGui::Checkbox("Player 1 female", &player1_choice);
	ImGui::Checkbox("Main menu", &on_main_menu);
	ImGui::Checkbox("Fire in the hole", &disable_hole);
}

//Use this for linking JUST GO automatically
void WorldManager::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	//public_gameobjects.push_back(&health_bar);
	//variable_names.push_back(GET_VARIABLE_NAME(health_bar));

	//public_gameobjects.push_back(&lose_screen);
	//variable_names.push_back(GET_VARIABLE_NAME(lose_screen));

	//public_gameobjects.push_back(&win_screen);
	//variable_names.push_back(GET_VARIABLE_NAME(win_screen));


	for (unsigned i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

void WorldManager::Save(Config& config) const
{
	config.AddBool(on_main_menu, "Main Menu");
}

void WorldManager::Load(const Config& config)
{
	on_main_menu = config.GetBool("Main Menu", false);
}

bool WorldManager::LoadLevel() const
{
	if(singleplayer)
	{
		App->scene->LoadScene(0);
		return true;
	}
	else
	{

		//Get players
		App->scene->LoadScene(0);
		return true;
		
	}

	return false;
}

void WorldManager::InitTriggers()
{
	GameObject* trigger_go_dad = App->scene->GetGameObjectByName("Triggers");
	for(size_t i = 0; i < trigger_go_dad->children.size();++i)
	{
		event_triggers[i] = static_cast<ComponentCollider*>(trigger_go_dad->children[i]->GetComponent(ComponentCollider::ColliderType::BOX));
	}
}

void WorldManager::CheckTriggers()
{
	for(size_t i = 0; i < 3; ++i)
	{
		if (static_cast<ComponentCollider*>(player1_go->GetComponent(ComponentCollider::ColliderType::CAPSULE))->DetectCollisionWith(event_triggers[i]))
		{
			if(!event_manager->events_triggered[i])
			{
				event_manager->TriggerEvent(i);
			}
		}
	}
}

void WorldManager::CheckHole()
{
	if (singleplayer)
	{
		if(!player1_choice)
		{
			disable_hole = hole->DetectCollisionWith(static_cast<ComponentCollider*>(player1_go->GetComponent(Component::ComponentType::COLLIDER)));
		}
		else
		{
			disable_hole = hole->DetectCollisionWith(static_cast<ComponentCollider*>(player2_go->GetComponent(Component::ComponentType::COLLIDER)));
		}
	}
	else
	{
		disable_hole = hole->DetectCollisionWith(static_cast<ComponentCollider*>(player2_go->GetComponent(Component::ComponentType::COLLIDER))) &&
			hole->DetectCollisionWith(static_cast<ComponentCollider*>(player1_go->GetComponent(Component::ComponentType::COLLIDER)));
	}

	if (disable_hole)
	{
		hole->owner->SetEnabled(false);
		if (singleplayer)
		{
			if (!player1_choice)
			{
				player1_controller->MakePlayerFall(fall);
			}
			else
			{
				player2_controller->MakePlayerFall(fall);
			}
		}
		else
		{
			player1_controller->MakePlayerFall(fall);
			player2_controller->MakePlayerFall(fall);

		}
		
	}

}

bool WorldManager::CheckLose()
{
	if (singleplayer && !was_multiplayer)
	{
		//If player1_choice == 0 he is chosing male model
		if (!player1_choice)
		{
			if (!player1_controller->is_alive)
			{
				return true;
			}
		}
		//Chosing female model
		else
		{
			if (!player2_controller->is_alive)
			{
				return true;
			}
		}
	}
	//Multiplayer
	else
	{
		if ((!player1_controller->is_alive || !player2_controller->is_alive) && !was_multiplayer)
		{
			camera_manager->MultiplayerToSingleplayer();
			singleplayer = true;
			was_multiplayer = true;


			return false;
		}

		if (!player1_controller->is_alive && !player2_controller->is_alive)
		{
			return true;
		}
	}

	return false;
}

//Use this for linking GO AND VARIABLES automatically if you need to save variables
// void WorldManager::Save(Config& config) const
// {
//	config.AddBool(on_main_menu, "OnMainMenu");
// 	Script::Save(config);
// }
//
//// //Use this for linking GO AND VARIABLES automatically
// void WorldManager::Load(const Config& config)
// {
// 	on_main_menu = config.GetUInt("OnMainMenu", 0);
// 	Script::Load(config);
// }
