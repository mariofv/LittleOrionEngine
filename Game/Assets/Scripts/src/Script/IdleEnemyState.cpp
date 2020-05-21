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

	enemy->target_on_idle = enemy->current_target->owner;
}

void IdleEnemyState::OnStateUpdate()
{
	//if (!enemy->animation->IsOnState("Idle"))
	//{
	//	enemy->animation->ActiveAnimation("idle");
	//}

	//enemy->GetClosestTarget();

	if (enemy->PlayerInSight() /*&& (enemy->SlotsAvailable() || enemy->current_target != enemy->target_on_idle)*/)
	{
		Exit(enemy->pursue_state);
	}
}

void IdleEnemyState::OnStateExit()
{
	enemy->target_on_idle = enemy->current_target->owner;
}
