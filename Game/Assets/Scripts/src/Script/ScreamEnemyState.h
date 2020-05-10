#ifndef  __SCREAMENEMYSTATE_H__
#define  __SCREAMENEMYSTATE_H__

#include "EnemyState.h"

class ScreamEnemyState : public EnemyState
{
public:
	ScreamEnemyState();
	ScreamEnemyState(Mushdoom* enemy);
	~ScreamEnemyState() = default;

	void OnStateEnter() override;
	void OnStateUpdate() override;
	void OnStateExit() override;
};

extern "C" SCRIPT_API ScreamEnemyState* ScreamEnemyStateDLL(); //This is how we are going to load the script
#endif