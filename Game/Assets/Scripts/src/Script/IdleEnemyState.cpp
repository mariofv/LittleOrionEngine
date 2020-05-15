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
}

void IdleEnemyState::OnStateUpdate()
{
	if (enemy->PlayerInSight())
	{
		Exit(enemy->pursue_state);
	}
}

void IdleEnemyState::OnStateExit()
{
}
