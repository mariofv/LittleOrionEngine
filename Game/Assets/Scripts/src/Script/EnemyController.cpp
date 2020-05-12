#include "EnemyController.h"

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
	Move();
}

// Use this for showing variables on inspector
void EnemyController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();

	ImGui::Text("Enemy Stats");
	ImGui::InputFloat("Rot Speed", &rot_speed);
	ImGui::InputFloat("Move Speed", &move_speed);
	ImGui::InputFloat("Attack Speed", &attack_speed);
	ImGui::InputFloat("Attack Power", &attack_power);
	ImGui::InputFloat("Attack Range", &attack_range);
	ImGui::InputFloat("Max Health", &max_health_points);
	ImGui::InputFloat("Health Points", &health_points);
	ImGui::InputFloat("Stop Distance", &stop_distance);
	ImGui::InputFloat("Detect Distance", &detect_distance);

	ImGui::NewLine();
	ImGui::Text("Enemy Debug");
	ImGui::Checkbox("Is Alive", &is_alive);
	ImGui::Checkbox("Is Attacking", &is_attacking);
}

//Use this for linking JUST GO automatically 
void EnemyController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	//public_gameobjects.push_back(&object_collider);
	//variable_names.push_back(GET_VARIABLE_NAME(object_collider));

	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

void EnemyController::TakeDamage(float damage)
{
	health_points -= damage;

	if (health_points <= 0)
	{
		Die();
	}
}

void EnemyController::InitMembers()
{
	GameObject* enemy_manager_go = App->scene->GetGameObjectByName("EnemyManager");
	ComponentScript* enemy_manager_component = enemy_manager_go->GetComponentScript("EnemyManager");
	enemy_manager = static_cast<EnemyManager*>(enemy_manager_component->script);

	player = App->scene->GetGameObjectByName("Player");

	animation = (ComponentAnimation*)owner->GetComponent(Component::ComponentType::ANIMATION);

	init_translation = owner->transform.GetTranslation();
	init_rotation = owner->transform.GetRotation();
	init_scale = owner->transform.GetScale();

	//if (object_collider != nullptr)
	collider = static_cast<ComponentCollider*>(owner->GetComponent(Component::ComponentType::COLLIDER));
}

void EnemyController::Move()
{
	if (!PlayerInSight())
	{
		if (!animation->IsOnState("Idle"))
		{
			animation->ActiveAnimation("idle");
		}

		return;
	}

	const float3 player_transform = player->transform.GetTranslation();
	float3 transform = owner->transform.GetTranslation();

	float3 direction = player_transform - transform;

	// If player in range attack
	if (PlayerInRange())
	{
		if (!animation->IsOnState("Attack"))
		{
			animation->ActiveAnimation("attack");
		}
	}
	// If player not in range walk
	else
	{
		// If player still attacking, finish animation and then switch to walking
		if (animation->IsOnState("Attack"))
		{
			return;
		}
		else if (animation->IsOnState("Idle"))
		{
			animation->ActiveAnimation("pursue");
			return;
		}

		float3 position = transform + (direction.Normalized() * move_speed);

		float3 next_position;
		bool valid_position = App->artificial_intelligence->FindNextPolyByDirection(position, next_position);

		if (valid_position)
		{
			position.y = next_position.y;
		}

		if (App->artificial_intelligence->IsPointWalkable(position))
		{
			owner->transform.LookAt(position);
			owner->transform.SetTranslation(position);
		}
	}
}

bool EnemyController::PlayerInSight()
{
	return player->transform.GetTranslation().Distance(owner->transform.GetTranslation()) < detect_distance;
}

bool EnemyController::PlayerInRange()
{
	return player->transform.GetTranslation().Distance(owner->transform.GetTranslation()) <= stop_distance;
}

float EnemyController::GetMoveSpeed() const
{
	return move_speed;
}

bool EnemyController::PointInNavMesh(float3& position, float3& next_position)
{
	return App->artificial_intelligence->FindNextPolyByDirection(position, next_position);
}

bool EnemyController::IsPointWalkable(float3& position)
{
	return App->artificial_intelligence->IsPointWalkable(position);
}

void EnemyController::LookAndMoveToPoint(float3 & position)
{
	owner->transform.LookAt(position);
	owner->transform.SetTranslation(position);
}

void EnemyController::LookToPoint(float3 & position)
{
	owner->transform.LookAt(position);
}

void EnemyController::SetVelocity(float3 & position)
{
	collider->SetVelocity(position, move_speed);
}

void EnemyController::Die()
{
	is_alive = false;
	enemy_manager->KillEnemy(this);
}
