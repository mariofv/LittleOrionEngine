#ifndef  __ENEMYSTATE_H__
#define  __ENEMYSTATE_H__

#include "Script.h"

class Mushdoom;

class EnemyState : public Script
{
public:
	EnemyState();
	EnemyState(Mushdoom* enemy);
	virtual ~EnemyState() = default;

	virtual void OnStateEnter() {}
	virtual void OnStateUpdate() {}
	virtual void OnStateExit() {}

	void Exit(EnemyState* state);

public:
	const char* name;

protected:
	Mushdoom* enemy = nullptr;
};
extern "C" SCRIPT_API EnemyState* EnemyStateDLL(); //This is how we are going to load the script
#endif