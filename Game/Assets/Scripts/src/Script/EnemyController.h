#ifndef  __ENEMYCONTROLLER_H__
#define  __ENEMYCONTROLLER_H__
#include "Script.h"

#include "EnemyManager.h"

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

	EnemyManager* punterito = nullptr;

private:
	void Death();

private:
	GameObject* player = nullptr;	float health_points = 100.0f;

};
extern "C" SCRIPT_API EnemyController* EnemyControllerDLL(); //This is how we are going to load the script
#endif