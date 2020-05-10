#ifndef  __ENEMYCONTROLLER_H__
#define  __ENEMYCONTROLLER_H__

#include "Script.h"

#include "EnemyManager.h"

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
	void TakeDamage(float damage);
	void InitMembers();
	virtual void ResetEnemy() {}

protected:
	void Move();
	bool Attack() {}
	bool PlayerInSight();
	void OnCollisionEnter() {}
	void Die();

public:
	GameObject* player = nullptr;
	ComponentCollider* collider_component = nullptr;
	bool is_alive = true;

protected:
	EnemyType type;
	ComponentAnimation* animation = nullptr;
	EnemyManager* enemy_manager = nullptr;

	float rot_speed = 0.01f;
	float move_speed = 0.2f;
	float attack_speed = 1.f;
	float attack_power = 1.f;
	float attack_range = 1.f;
	float max_health_points = 100.f; //not const, to modify in inspector
	float health_points = max_health_points;
	float stopping_distance = 2.f;
	float detect_player_distance = 100.f;

	bool is_attacking = false;

	float3 init_translation;
	Quat init_rotation;
	float3 init_scale;
};

extern "C" SCRIPT_API EnemyController* EnemyControllerDLL(); //This is how we are going to load the script

#endif