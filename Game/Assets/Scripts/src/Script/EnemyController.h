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

	bool PlayerInSight();
	bool PlayerInRange();

	float GetMoveSpeed() const;
	bool PointInNavMesh(float3& position, float3& next_position);
	bool IsPointWalkable(float3& position);
	void LookAndMoveToPoint(float3& position);
	void SetVelocity(float3& position);

protected:
	void Move();
	bool Attack() {}

	void OnCollisionEnter() {}
	void Die();

public:
	GameObject* player = nullptr;
	ComponentAnimation* animation = nullptr;
	bool is_alive = true;

	//GameObject* object_collider = nullptr;
	ComponentCollider* collider = nullptr;

protected:
	EnemyType type;

	EnemyManager* enemy_manager = nullptr;

	float rot_speed = 0.01f;
	float move_speed = 0.01f;
	float attack_speed = 1.f;
	float attack_power = 1.f;
	float attack_range = 1.f;
	float max_health_points = 100.f; //not const, to modify in inspector
	float health_points = max_health_points;
	float stop_distance = 2.f;
	float detect_distance = 10.f;

	bool is_attacking = false;

	float3 init_translation;
	Quat init_rotation;
	float3 init_scale;
};

extern "C" SCRIPT_API EnemyController* EnemyControllerDLL(); //This is how we are going to load the script

#endif