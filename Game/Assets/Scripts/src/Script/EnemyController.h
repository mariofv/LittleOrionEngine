#ifndef  __ENEMYCONTROLLER_H__
#define  __ENEMYCONTROLLER_H__

#include "Script.h"

#include "EnemyManager.h"

class ComponentAnimation;

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

protected:
	void Move();
	bool Attack() {}
	bool PlayerInSight();
	void OnCollisionEnter() {}
	void Die();
	virtual void ResetEnemy() {}

protected:
	EnemyType type;
	GameObject* player = nullptr;
	ComponentAnimation* animation = nullptr;

	float rot_speed = 0.01f;
	float move_speed = 0.2f;
	float attack_speed = 1.f;
	float attack_power = 1.f;
	float attack_range = 1.f;
	float health_points = 100.f;
	float stopping_distance = 2.f;
	float detect_player_distance = 100.f;

	bool is_dead = false;
	bool is_attacking = false;
};

extern "C" SCRIPT_API EnemyController* EnemyControllerDLL(); //This is how we are going to load the script

#endif