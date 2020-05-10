#include "EnemyState.h"

#include "Mushdoom.h"

EnemyState* EnemyStateDLL()
{
	EnemyState* instance = new EnemyState();
	return instance;
}

EnemyState::EnemyState()
{
}

EnemyState::EnemyState(Mushdoom* enemy) : enemy(enemy)
{
}

void EnemyState::Exit(EnemyState* state)
{
	enemy->current_state->OnStateExit();
	enemy->current_state = state;
	enemy->current_state->OnStateEnter();
}
