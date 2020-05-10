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

	if(current_number_of_enemies_alive < max_enemies_alive && App->input->GetKeyDown(KeyCode::F))
	{
		SpawnEnemy(0, spawn_points[rand() % 3]);
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
	enemy->owner->SetEnabled(false);
	enemy->owner->transform.SetTranslation(graveyard_position);


	//Reset enemy
	enemy->ResetEnemy();
	enemy->is_alive = false;
	--current_number_of_enemies_alive;
}

void EnemyManager::SpawnEnemy(const unsigned type, const float3& spawn_position)
{
	//We only will have mushdoom for vs2
	EnemyController* enemy = nullptr;

	for(size_t i = 0; i < enemies.size(); ++i)
	{
		if(!enemies[i]->is_alive) // also have to match with the enemy type 
		{
			enemy = enemies[i];
			enemy->is_alive = true;
			enemy->owner->transform.SetTranslation(spawn_position);
			enemy->owner->SetEnabled(true);

			++current_number_of_enemies_alive;

			break;
		} 
		
	}
	assert(enemy != nullptr);
}

void EnemyManager::CreateEnemies()
{
	//For now we only have mushdoom enemy
	size_t number_of_instances = MAX_NUMBER_OF_MUSHDOOM - enemies.size();
	for (size_t i = 0; i < number_of_instances; ++i)
	{
		GameObject* duplicated_go = App->scene->DuplicateGameObject(mushdoom_go, mushdoom_go->parent);

		const ComponentScript* componnet_enemy = duplicated_go->GetComponentScript("Mushdoom");
		Mushdoom* enemy = (Mushdoom*)componnet_enemy->script;
		enemy->InitMembers();
		enemy->is_alive = false;
		enemies.emplace_back(enemy);
		enemy->owner->transform.SetTranslation(graveyard_position);
		//enemy->owner->SetEnabled(false);
	}
}

// Use this for showing variables on inspector
void EnemyManager::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

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