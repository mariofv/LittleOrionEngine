#ifndef  __MUSHDOOM_H__
#define  __MUSHDOOM_H__

#include "EnemyController.h"

class EnemyState;
class IdleEnemyState;
class ScreamEnemyState;
class PursueEnemyState;
class AttackEnemyState;

class Mushdoom : public EnemyController
{
public:
	Mushdoom();
	~Mushdoom();

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;
	//void InitPublicGameObjects();

	void ResetEnemy() override;

public:
	EnemyState* current_state = nullptr;
	EnemyState* idle_state = nullptr;
	EnemyState* scream_state = nullptr;
	EnemyState* pursue_state = nullptr;
	EnemyState* attack_state = nullptr;

public:

};
extern "C" SCRIPT_API Mushdoom* MushdoomDLL(); //This is how we are going to load the script
#endif