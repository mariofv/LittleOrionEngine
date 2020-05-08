#ifndef  __ENEMYCONTROLLER_H__
#define  __ENEMYCONTROLLER_H__
#include "Script.h"

#include "EnemyManager.h"

class ComponentAnimation;

class EnemyController : public Script
{
public:
	EnemyController();
	~EnemyController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();	void TakeDamage(float damage);

	//virtual void ResetEnemy() = 0;

private:
	void Death();
public:
	bool is_alive = true;
	GameObject* player = nullptr;
private:
	float health_points = 100.0f;
	EnemyManager* enemy_manager = nullptr;
	ComponentAnimation* animation = nullptr;

};
extern "C" SCRIPT_API EnemyController* EnemyControllerDLL(); //This is how we are going to load the script
#endif