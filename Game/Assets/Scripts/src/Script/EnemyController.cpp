#include "EnemyController.h"
#include "Component/ComponentAnimation.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

EnemyController* EnemyControllerDLL()
{
	EnemyController* instance = new EnemyController();
	return instance;
}

EnemyController::EnemyController()
{
	panel = new PanelComponent();
}


// Use this for initialization before Start()
void EnemyController::Awake()
{
	InitMembers();
	enemy_manager->AddEnemy(this);
}

// Use this for initialization
void EnemyController::Start()
{}

// Update is called once per frame
void EnemyController::Update()
{
}

// Use this for showing variables on inspector
void EnemyController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
}

//Use this for linking JUST GO automatically 
void EnemyController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	public_gameobjects.push_back(&player);
	variable_names.push_back(GET_VARIABLE_NAME(player));
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

void EnemyController::TakeDamage(float damage)
{
	health_points -= damage;

	if(health_points <= 0.0f)
	{
		Death();
		//F
	}
}

void EnemyController::InitMembers()
{
	GameObject* enemy_manager_go = App->scene->GetGameObjectByName("EnemyManager");
	ComponentScript* enemy_manager_component = enemy_manager_go->GetComponentScript("EnemyManager");
	enemy_manager = static_cast<EnemyManager*>(enemy_manager_component->script);
	animation = static_cast<ComponentAnimation*>(owner->GetComponent(Component::ComponentType::ANIMATION));
}

void EnemyController::Death()
{
	enemy_manager->KillEnemy(this);
}


