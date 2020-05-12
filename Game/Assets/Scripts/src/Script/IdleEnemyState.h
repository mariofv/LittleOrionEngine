#ifndef  __IDLEENEMYSTATE_H__
#define  __IDLEENEMYSTATE_H__

#include "EnemyState.h"

class IdleEnemyState : public EnemyState
{
public:
	IdleEnemyState();
	IdleEnemyState(Mushdoom* enemy);
	~IdleEnemyState() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;
};

extern "C" SCRIPT_API IdleEnemyState* IdleEnemyStateDLL(); //This is how we are going to load the script
#endif