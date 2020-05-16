#include "EnemyController.h"

#include "Component/ComponentCollider.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentAnimation.h"
#include "Component/ComponentCollider.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleAI.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "PlayerController.h"

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
{
}

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

	ImGui::Text("Enemy Stats");
	ImGui::InputFloat("Move Speed", &move_speed);
	ImGui::InputFloat("Rotate Speed", &rotate_speed);
	ImGui::InputFloat("Attack Speed", &attack_speed);
	ImGui::InputFloat("Attack Power", &attack_power);
	ImGui::InputFloat("Attack Range", &attack_range);
	ImGui::InputFloat("Max Health", const_cast<float*>(&MAX_HEALTH_POINTS));
	ImGui::InputFloat("Health Points", &health_points);
	ImGui::InputFloat("Detect Distance", &detect_distance);

	ImGui::NewLine();
	ImGui::Text("Enemy Flags");
	ImGui::Checkbox("Is Alive", &is_alive);
	ImGui::Checkbox("Is Attacking", &is_attacking);
	ImGui::Checkbox("Move with Physics", &move_with_physics);
}

//Use this for linking JUST GO automatically 
void EnemyController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

void EnemyController::InitMembers()
{
	GameObject* enemy_manager_go = App->scene->GetGameObjectByName("EnemyManager");
	ComponentScript* enemy_manager_component = enemy_manager_go->GetComponentScript("EnemyManager");
	enemy_manager = static_cast<EnemyManager*>(enemy_manager_component->script);

	animation = static_cast<ComponentAnimation*>(owner->GetComponent(Component::ComponentType::ANIMATION));
	collider = static_cast<ComponentCollider*>(owner->GetComponent(Component::ComponentType::COLLIDER));

	attack_collider = static_cast<ComponentCollider*>(attack_detector->GetComponent(Component::ComponentType::COLLIDER));

	player = App->scene->GetGameObjectByName("Player");
	ComponentScript* player_component = player->GetComponentScript("PlayerController");
	player_controller = static_cast<PlayerController*>(player_component->script);

	init_translation = owner->transform.GetTranslation();
	init_rotation = owner->transform.GetRotation();
	init_scale = owner->transform.GetScale();
}

bool EnemyController::PlayerInSight()
{
	return player->transform.GetTranslation().Distance(owner->transform.GetTranslation()) < detect_distance;
}

bool EnemyController::PlayerInRange()
{
	return player->transform.GetTranslation().Distance(owner->transform.GetTranslation()) <= attack_range;
}

void EnemyController::MoveTowardsPlayer()
{
	const float3 player_translation = player->transform.GetTranslation();
	float3 translation = owner->transform.GetTranslation();

	float3 direction = player_translation - translation;

	float speed = (move_with_physics) ? move_speed : move_speed / 50.f;
	float3 position = translation + (direction.Normalized() * speed);

	float3 next_position;
	if (App->artificial_intelligence->FindNextPolyByDirection(position, next_position))
	{
		position.y = next_position.y;
	}

	if (App->artificial_intelligence->IsPointWalkable(position))
	{
		if (move_with_physics)
		{
			float3 direction_norm = direction.Normalized();
			collider->SetVelocity(direction_norm, move_speed);
		}
		else
		{
			owner->transform.LookAt(position);
			owner->transform.SetTranslation(position);
		}
	}
}

void EnemyController::Attack()
{
	player_controller->TakeDamage(attack_damage);
}

void EnemyController::TakeDamage(float damage)
{
	health_points -= damage;

	if (health_points <= 0)
	{
		Die();
	}
}

void EnemyController::Die()
{
	is_alive = false;
	enemy_manager->KillEnemy(this);
}
