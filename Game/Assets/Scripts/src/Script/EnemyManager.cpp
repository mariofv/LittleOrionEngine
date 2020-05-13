#include "EnemyManager.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleScriptManager.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"
#include <stdlib.h> 
#include <time.h>

#include "EventManager.h"
#include "Mushdoom.h"



EnemyManager* EnemyManagerDLL()
{
	EnemyManager* instance = new EnemyManager();
	return instance;
}

EnemyManager::EnemyManager()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void EnemyManager::Awake()
{
	GameObject* event_manager_go = App->scene->GetGameObjectByName("EventManager");
	ComponentScript* event_manager_component = event_manager_go->GetComponentScript("EventManager");
	event_manager = static_cast<EventManager*>(event_manager_component->script);

	InitSpawnPoints();
}

// Use this for initialization
void EnemyManager::Start()
{
	CreateEnemies(); //For now we only create mushdooms but in the future we create enemies depending on which level we are
}

// Update is called once per frame
void EnemyManager::Update()
{
	srand(time(NULL));

	if(!enemies_spawning_queue.empty() && current_number_of_enemies_alive < MAX_NUMBER_OF_MUSHDOOM)
	{
		for(auto it = enemies_spawning_queue.begin(); it != enemies_spawning_queue.end(); ++it)
		{
			if(CheckSpawnAvailability(*it))
			{
				SpawnEnemy(0, *it);
				enemies_spawning_queue.erase(it);
				break;
			}
		}
	}
}

void EnemyManager::AddEnemy(EnemyController* enemy)
{
	enemies.emplace_back(enemy);
	++current_number_of_enemies_alive;
}

void EnemyManager::KillEnemy(EnemyController* enemy)
{
	//This method is called once the enemy animation ended
	enemy->collider->SwitchPhysics(true);
	enemy->owner->transform.SetTranslation(graveyard_position);
	enemy->owner->SetEnabled(false);

	//Reset enemy
	enemy->ResetEnemy();
	--current_number_of_enemies_alive;

	if(event_manager->event_triggered)
	{
		++event_manager->enemies_killed_on_wave;
	}
}

void EnemyManager::SpawnEnemy(const unsigned type, const float3& spawn_position)
{
	//We only will have mushdoom for vs2
	Mushdoom* enemy = nullptr;

	for(size_t i = 0; i < enemies.size(); ++i)
	{
		if(!enemies[i]->is_alive) // also have to match with the enemy type 
		{
			enemy = static_cast<Mushdoom*>(enemies[i]);
			enemy->is_alive = true;
			enemy->Start();
			enemy->owner->transform.SetTranslation(spawn_position);
			enemy->collider->UpdateDimensions();
			enemy->collider->SwitchPhysics(false);
			enemy->owner->SetEnabled(true);

			++current_number_of_enemies_alive;

			break;
		} 
		
	}
	assert(enemy != nullptr);
}

void EnemyManager::SpawnWave(unsigned event, unsigned enemies_per_wave)
{

	switch (event)
	{
		case 0:
			for(size_t i = 0; i < enemies_per_wave; ++i)
			{
				enemies_spawning_queue.emplace_back(spawn_points[0]);
			}
			break;
		case 1:
			for (size_t i = 0; i < enemies_per_wave; ++i)
			{
				int random_spawn = (rand() % 2) + 1;
				enemies_spawning_queue.emplace_back(spawn_points[random_spawn]);
			}
			break;
		case 2:
			for (size_t i = 0; i < enemies_per_wave; ++i)
			{
				int random_spawn = (rand() % 2) + 3;
				enemies_spawning_queue.emplace_back(spawn_points[random_spawn]);
			}
			break;
		default:
			return;
	}
}

void EnemyManager::CreateEnemies()
{
	//static_cast<ComponentCollider*>(mushdoom_go->GetComponent(Component::ComponentType::COLLIDER))->SwitchPhysics(true);

	//For now we only have mushdoom enemy
	size_t number_of_instances = MAX_NUMBER_OF_MUSHDOOM - enemies.size();
	for (size_t i = 0; i < number_of_instances; ++i)
	{
		GameObject* duplicated_go = App->scene->DuplicateGameObject(mushdoom_go, mushdoom_go->parent);

		const ComponentScript* componnet_enemy = duplicated_go->GetComponentScript("Mushdoom");
		Mushdoom* enemy = (Mushdoom*)componnet_enemy->script;
		enemy->InitMembers();
		enemy->is_alive = false;
		enemy->collider->SwitchPhysics(true);
		enemy->owner->SetEnabled(false);
		enemy->owner->transform.SetTranslation(graveyard_position);
		enemies.emplace_back(enemy);
	}
}

// Use this for showing variables on inspector
void EnemyManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Text("enemies_spawning_queue size:  %d", enemies_spawning_queue.size());

}

//Use this for linking JUST GO automatically 
void EnemyManager::InitPublicGameObjects()
{

	public_gameobjects.push_back(&mushdoom_go);
	variable_names.push_back(GET_VARIABLE_NAME(mushdoom_go));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
void EnemyManager::InitSpawnPoints()
{
	GameObject* spawn_go_dad = App->scene->GetGameObjectByName("Spawns");
	for (size_t i = 0; i < spawn_go_dad->children.size(); ++i)
	{
		spawn_points[i] = spawn_go_dad->children[i]->transform.GetTranslation();
	}
}
bool EnemyManager::CheckSpawnAvailability(float3 & spawn_position)
{
	for(const auto& enemy : enemies)
	{
		if(!enemy->is_alive)
		{
			continue;
		}

		if(enemy->owner->transform.GetTranslation().Distance(spawn_position) <= 10.0f)
		{
			return false;
		}
	}

	return true;
}
//Use this for linking GO AND VARIABLES automatically if you need to save variables 
// void EnemyManager::Save(Config& config) const
// {
// 	config.AddUInt(example->UUID, "ExampleNameforSave");
// 	Script::Save(config);
// }

// //Use this for linking GO AND VARIABLES automatically
// void EnemyManager::Load(const Config& config)
// {
// 	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
// 	Script::Load(config);
// }