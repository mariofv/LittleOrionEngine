#ifndef  __ENEMYCONTROLLER_H__
#define  __ENEMYCONTROLLER_H__

#include "Script.h"

#include "EnemyManager.h"
#include "PlayerController.h"

class ComponentAnimation;
class ComponentCollider;

enum class EnemyType
{
	MUSHDOOM
};

class EnemyController : public Script
{
public:
	EnemyController();
	~EnemyController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	void InitMembers();

	bool PlayerInSight();
	bool PlayerInRange();
	void SeekPlayer();
	void SeekPlayerWithSeparation();
	void TakeDamage(float damage);
	bool SlotsAvailable();

	virtual void ResetEnemy() {}

	void UpdateCurrentPlayerTarget();

protected:
	void Die();

public:
	ComponentAnimation* animation = nullptr;
	ComponentCollider* collider = nullptr;

	GameObject* current_player_target = nullptr;
	GameObject* current_player_target_on_idle = nullptr;

	bool is_alive = true;
	bool is_attacking = false;

protected:
	EnemyType type;

	GameObject* player1 = nullptr;
	GameObject* player2 = nullptr;

	EnemyManager* enemy_manager = nullptr;
	PlayerController* player1_controller = nullptr;
	PlayerController* player2_controller = nullptr;

	float move_speed = 1.f;
	float rotate_speed = 1.f;
	float attack_speed = 1.f;
	float attack_power = 10.f;
	float attack_range = 1.4f;
	const float MAX_HEALTH_POINTS = 100.f;
	float health_points = MAX_HEALTH_POINTS;
	float detect_distance = 50.f;
	float switch_target_distance = 0.5f;
	float attack_radius = 2.2f;
	float separation_distance = 2.f;

	bool move_with_physics = true;

	float3 init_translation;
	Quat init_rotation;
	float3 init_scale;
};

extern "C" SCRIPT_API EnemyController* EnemyControllerDLL(); //This is how we are going to load the script

#endif