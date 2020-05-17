#ifndef  __DIEENEMYSTATE_H__
#define  __DIEENEMYSTATE_H__

#include "EnemyState.h"

class DieEnemyState : public EnemyState
{
public:
	DieEnemyState();
	DieEnemyState(Mushdoom* enemy);
	~DieEnemyState() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;
};
extern "C" SCRIPT_API DieEnemyState* DieEnemyStateDLL(); //This is how we are going to load the script
#endif