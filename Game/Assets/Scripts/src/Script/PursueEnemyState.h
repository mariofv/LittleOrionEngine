#ifndef  __PURSUEENEMYSTATE_H__
#define  __PURSUEENEMYSTATE_H__

#include "EnemyState.h"

class PursueEnemyState : public EnemyState
{
public:
	PursueEnemyState();
	PursueEnemyState(Mushdoom* enemy);
	~PursueEnemyState() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;
};

extern "C" SCRIPT_API PursueEnemyState* PursueEnemyStateDLL(); //This is how we are going to load the script
#endif