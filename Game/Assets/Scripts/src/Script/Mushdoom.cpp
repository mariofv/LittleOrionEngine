#include "Mushdoom.h"

#include "EnemyState.h"
#include "IdleEnemyState.h"
#include "ScreamEnemyState.h"
#include "PursueEnemyState.h"
#include "AttackEnemyState.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

Mushdoom* MushdoomDLL()
{
	Mushdoom* instance = new Mushdoom();
	return instance;
}

Mushdoom::Mushdoom()
{
	panel = new PanelComponent();

	type = EnemyType::MUSHDOOM;

	current_state = new EnemyState();
	idle_state = new IdleEnemyState(this);
	scream_state = new ScreamEnemyState(this);
	pursue_state = new PursueEnemyState(this);
	attack_state = new AttackEnemyState(this);
}

Mushdoom::~Mushdoom()
{
	delete current_state;
	delete idle_state;
	delete scream_state;
	delete pursue_state;
	delete attack_state;
}

// Use this for initialization before Start()
void Mushdoom::Awake()
{
	EnemyController::Awake();
}

// Use this for initialization
void Mushdoom::Start()
{
	current_state = idle_state;
	current_state->OnStateEnter();
}

// Update is called once per frame
void Mushdoom::Update()
{
	//EnemyController::Update();
	current_state->OnStateUpdate();
}

void Mushdoom::ResetEnemy()
{
	health_points = max_health_points;

	is_alive = true;
	is_attacking = false;

	owner->transform.SetTranslation(init_translation);
	owner->transform.SetRotation(init_rotation);
	owner->transform.SetScale(init_scale);
}

//Use this for showing variables on inspector
void Mushdoom::OnInspector(ImGuiContext* context)
{
	EnemyController::OnInspector(context);

	//ImGui::Text("Current State: %s", current_state->name);
}

////Use this for linking JUST GO automatically 
//void Mushdoom::InitPublicGameObjects()
//{
//	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
//
//	for (int i = 0; i < public_gameobjects.size(); ++i)
//	{
//		name_gameobjects.push_back(is_object);
//		go_uuids.push_back(0);
//	}
//}
