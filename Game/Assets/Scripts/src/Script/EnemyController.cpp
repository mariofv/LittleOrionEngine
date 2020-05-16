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
#include "Module/ModuleTime.h"

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
	ImGui::InputFloat("Target Distance", &switch_target_distance);
	ImGui::InputFloat("Separation Distance", &separation_distance);

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
	const GameObject* enemy_manager_go = App->scene->GetGameObjectByName("EnemyManager");
	const ComponentScript* enemy_manager_component = enemy_manager_go->GetComponentScript("EnemyManager");
	enemy_manager = static_cast<EnemyManager*>(enemy_manager_component->script);

	animation = static_cast<ComponentAnimation*>(owner->GetComponent(Component::ComponentType::ANIMATION));
	collider = static_cast<ComponentCollider*>(owner->GetComponent(Component::ComponentType::COLLIDER));

	player1 = App->scene->GetGameObjectByName("Player");
	if (player1 != nullptr)
	{
		const ComponentScript* player1_controller_component = player1->GetComponentScript("PlayerController");
		player1_controller = static_cast<PlayerController*>(player1_controller_component->script);
	}

	player2 = App->scene->GetGameObjectByName("Player2");
	if (player2 != nullptr)
	{
		const ComponentScript* player2_controller_component = player2->GetComponentScript("PlayerController");
		player2_controller = static_cast<PlayerController*>(player2_controller_component->script);
	}

	current_target = player1;

	init_translation = owner->transform.GetTranslation();
	init_rotation = owner->transform.GetRotation();
	init_scale = owner->transform.GetScale();
}

bool EnemyController::PlayerInSight()
{
	return current_target->transform.GetTranslation().Distance(owner->transform.GetTranslation()) < detect_distance;
}

bool EnemyController::PlayerInRange()
{
	float3 target = current_target->transform.GetTranslation() + (current_target->transform.GetFrontVector());
	return target.Distance(owner->transform.GetTranslation()) <= attack_range;
}

void EnemyController::SeekPlayer()
{
	if (!IsGrounded()) return;

	GetClosestTarget();

	float3 target = current_target->transform.GetTranslation(); //+ (current_player_target->transform.GetFrontVector());
	float3 position = owner->transform.GetTranslation();

	float3 desired_velocity = target - position;
	float speed = (move_with_physics) ? move_speed : move_speed / 40.f;

	position = position + (desired_velocity.Normalized() * speed);

	float3 next_position = float3::zero;
	if (App->artificial_intelligence->FindNextPolyByDirection(position, next_position))
	{
		position.y = next_position.y;
	}

	if (App->artificial_intelligence->IsPointWalkable(position))
	{
		if (move_with_physics)
		{
			float3 desired_velocity_normalized = desired_velocity.Normalized();
			collider->SetVelocity(desired_velocity_normalized, speed);
		}
		else
		{
			owner->transform.LookAt(position);
			owner->transform.SetTranslation(position);
		}
	}
}

void EnemyController::SeekPlayerWithSeparation()
{
	GetClosestTarget();

	float3 target = current_target->transform.GetTranslation();
	float3 position = owner->transform.GetTranslation();

	float3 desired_velocity = target - position;
	float3 separation_velocity = float3::zero;

	for (auto& enemy : enemy_manager->enemies)
	{
		if (!enemy->is_alive)
		{
			continue;
		}

		if (enemy->owner->transform.GetTranslation().Distance(position) <= separation_distance)
		{
			separation_velocity += enemy->owner->transform.GetTranslation() - position;
		}
	}

	if (!separation_velocity.Equals(float3::zero))
	{
		desired_velocity = /*desired_velocity +*/ (separation_velocity * -1.f);
		

	}

	float speed = (move_with_physics) ? move_speed : move_speed / 40.f;

	position = position + (desired_velocity.Normalized() * speed);

	float3 next_position = float3::zero;
	if (App->artificial_intelligence->FindNextPolyByDirection(position, next_position))
	{
		position.y = next_position.y;
	}

	if (App->artificial_intelligence->IsPointWalkable(position))
	{
		if (move_with_physics)
		{
			float3 desired_velocity_normalized = desired_velocity.Normalized();
			collider->SetVelocityEnemy(desired_velocity_normalized, speed);
		}
		else
		{
			owner->transform.LookAt(position);
			owner->transform.SetTranslation(position);
		}
	}
}

void EnemyController::BattleCircleAI()
{
	if (!IsGrounded()) return;

	current_target = GetClosestTarget();

	float3 target = current_target->transform.GetTranslation();
	float3 position = owner->transform.GetTranslation();
	float3 desired_velocity = float3::zero;

	float distance = abs(position.DistanceSq(target));

	for (auto& enemy : enemy_manager->enemies)
	{
		if (!enemy->is_alive)
		{
			continue;
		}

		if (enemy->owner->transform.GetTranslation().Distance(position) <= separation_distance)
		{
			desired_velocity += enemy->owner->transform.GetTranslation() - position;
		}
	}

	bool avoid = !desired_velocity.Equals(float3::zero) && distance <= danger_distance;
	bool strafe = !avoid && !engage_player && distance <= attack_distance;
	bool attack = engage_player && distance <= attack_distance;

	if (strafe)
	{
		//TODO ask player or enemy manager if can attack
	}

	if (avoid)
	{
		Avoid(desired_velocity);
	}
	else if (strafe)
	{
		float direction = rand() % 1;
		if (direction == 0)
		{
			direction = -1.f;
		}

		Strafe(target, direction);
	}
	else if (attack)
	{
		is_attacking = true;
		// if 
	}
	else
	{
		desired_velocity = target - position;
		Seek(desired_velocity);
	}
}

void EnemyController::Seek(float3& velocity)
{
	// Movement logic
	float speed = (move_with_physics) ? move_speed : move_speed / 40.f;

	float3 position = owner->transform.GetTranslation() + (velocity.Normalized() * speed);

	float3 next_position = float3::zero;
	if (App->artificial_intelligence->FindNextPolyByDirection(position, next_position))
	{
		position.y = next_position.y;
	}

	if (App->artificial_intelligence->IsPointWalkable(position))
	{
		if (move_with_physics)
		{
			float3 velocity_normalized = velocity.Normalized();
			collider->SetVelocityEnemy(velocity_normalized, speed);
		}
		else
		{
			owner->transform.LookAt(position);
			owner->transform.SetTranslation(position);
		}
	}
}

void EnemyController::Avoid(float3& velocity)
{
	float3 avoid_velocity = velocity * -1;
	Seek(avoid_velocity);
}

void EnemyController::Strafe(float3& target_position, float direction)
{
	float3 perpendicular_vec = float3::unitY.Cross(target_position);
	float3 desired_velocity = perpendicular_vec * direction;
	Seek(desired_velocity);
}

void EnemyController::TakeDamage(float damage)
{
	health_points -= damage;

	if (health_points <= 0)
	{
		Die();
	}
}

bool EnemyController::SlotsAvailable()
{
	if (is_attacking) return true;

	for (auto& enemy : enemy_manager->enemies)
	{
		if (!enemy->is_alive)
		{
			continue;
		}

		if (current_target == enemy->current_target && enemy->is_attacking)
		{
			return false;
		}
	}

	return true;
}

GameObject* EnemyController::GetClosestTarget()
{
	if (player2 == nullptr)
	{
		current_target = player1;
	}
	else
	{
		if (player1_controller->is_alive && player2_controller->is_alive)
		{
			float3 position = owner->transform.GetTranslation();

			float distance_player1 = player1->transform.GetTranslation().Distance(position);
			float distance_player2 = player2->transform.GetTranslation().Distance(position);

			if (abs(distance_player1 - distance_player2) >= switch_target_distance)
			{
				if (distance_player1 < distance_player2)
				{
					current_target = player1;
				}
				else
				{
					current_target = player2;
				}
			}
		}
		else if (!player1_controller->is_alive && player2_controller->is_alive)
		{
			current_target = player2;
		}
		else if (player1_controller->is_alive && !player2_controller->is_alive)
		{
			current_target = player1;
		}
		else if (!player1_controller->is_alive && !player2_controller->is_alive)
		{
			current_target = player1;
		}
	}

	return current_target;
}

void EnemyController::Die()
{
	is_alive = false;
	enemy_manager->KillEnemy(this);
}

bool EnemyController::IsGrounded() const
{
	btVector3 origin = collider->body->getWorldTransform().getOrigin();

	btVector3 end = collider->body->getWorldTransform().getOrigin();
	end.setY(end.getY() - (collider->box_size.getY() / 2));

	return collider->RaycastHit(origin, end);
}
