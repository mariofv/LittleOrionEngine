#ifndef  __ATTACKENEMYSTATE_H__
#define  __ATTACKENEMYSTATE_H__

#include "EnemyState.h"

class AttackEnemyState : public EnemyState
{
public:
	AttackEnemyState();
	AttackEnemyState(Mushdoom* enemy);
	~AttackEnemyState() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;
};

extern "C" SCRIPT_API AttackEnemyState* AttackEnemyStateDLL(); //This is how we are going to load the script
#endif