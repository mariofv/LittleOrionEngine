#include "EnemyManager.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "EnemyController.h"

EnemyManager* EnemyManager::instance_singleton = nullptr;

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
	instance_singleton = static_cast<EnemyManager*>(owner->GetComponentScript("EnemyManager")->script);
	

	
}

// Use this for initialization
void EnemyManager::Start()
{
	//For now we only have mushdoom enemy
	size_t number_of_instances = MAX_NUMBER_OF_MUSHDOOM - current_number_of_enemies_alive;
	for(size_t i = 0; i < number_of_instances; ++i)
	{
		GameObject* duplicated_go = App->scene->DuplicateGameObject(mushdoom_go, mushdoom_go->parent);
		
		const ComponentScript* componnet_enemy = duplicated_go->GetComponentScript("EnemyController");
		EnemyController* enemy = (EnemyController*)componnet_enemy->script;
		enemies.emplace_back(enemy);
		enemy->owner->transform.SetTranslation(graveyard_position);
	}
}

// Update is called once per frame
void EnemyManager::Update()
{

	
}

EnemyManager* EnemyManager::GetInstance()
{
	return instance_singleton;
}

void EnemyManager::AddEnemy(EnemyController* enemy)
{
	enemies.emplace_back(enemy);
}

void EnemyManager::KillEnemy(EnemyController* enemy)
{
	//This method is called once the enemy animation ended
	enemy->owner->transform.SetTranslation(graveyard_position);


	//Reset enemy
	//enemy->ResetEnemy();
	enemy->owner->SetEnabled(false);
	enemy->is_alive = false;
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

			break;
		} 
		
	}
	assert(enemy != nullptr);
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