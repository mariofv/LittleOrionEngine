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

#include "PlayerController.h"


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
	if(!on_main_menu)
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
	
	singleplayer = true;
	player1_choice = false;
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

	InitTriggers();
}

// Use this for initialization
void WorldManager::Start()
{

}

// Update is called once per frame
void WorldManager::Update()
{
	if(on_main_menu)
	{
		return;
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

	if(event_manager->current_event > 2)
	{
		//We won the level!
		player1_controller->owner->SetEnabled(false);
		player2_controller->owner->SetEnabled(false);
		//win_component->Enable();
		transition = true;
	}
}

// Use this for showing variables on inspector
void WorldManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Checkbox("Singleplayer", &App->input->singleplayer_input);
	ImGui::Checkbox("Main menu", &on_main_menu);
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


bool WorldManager::LoadLevel() const
{
	if(singleplayer)
	{
		App->scene->LoadScene(0);
		return true;
	}
	else
	{
		//If players are different
		if(player1_choice ^ player2_choice)
		{
			//Get players
			App->scene->LoadScene(0);
			return true;
		}
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
	if(current_event_trigger > 2)
	{
		return;
	}

	if(static_cast<ComponentCollider*>(player1_go->GetComponent(ComponentCollider::ColliderType::CAPSULE))->DetectCollisionWith(event_triggers[current_event_trigger]))
	{
		if(event_manager->TriggerEvent(current_event_trigger))
		{
			++current_event_trigger;
		}
	}

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
