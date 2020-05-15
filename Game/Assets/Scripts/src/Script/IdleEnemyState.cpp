#include "IdleEnemyState.h"

#include "Mushdoom.h"

#include "Component/ComponentAnimation.h"

IdleEnemyState* IdleEnemyStateDLL()
{
	IdleEnemyState* instance = new IdleEnemyState();
	return instance;
}

IdleEnemyState::IdleEnemyState()
{
}

IdleEnemyState::IdleEnemyState(Mushdoom* enemy) : EnemyState(enemy)
{
	name = "Idle";
}

void IdleEnemyState::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Idle"))
	{
		enemy->animation->ActiveAnimation("idle");
	}

	enemy->current_player_target_on_idle = enemy->current_player_target;
}

void IdleEnemyState::OnStateUpdate()
{
	if (!enemy->animation->IsOnState("Idle"))
	{
		enemy->animation->ActiveAnimation("idle");
	}

	enemy->UpdateCurrentPlayerTarget();

	if (enemy->PlayerInSight() && (enemy->SlotsAvailable() || enemy->current_player_target != enemy->current_player_target_on_idle))
	{
		Exit(enemy->pursue_state);
	}
}

void IdleEnemyState::OnStateExit()
{
	enemy->current_player_target_on_idle = enemy->current_player_target;
}
