#include "AttackEnemyState.h"

#include "Mushdoom.h"

#include "Component/ComponentAnimation.h"

AttackEnemyState* AttackEnemyStateDLL()
{
	AttackEnemyState* instance = new AttackEnemyState();
	return instance;
}

AttackEnemyState::AttackEnemyState()
{
	panel = new PanelComponent();
}

AttackEnemyState::AttackEnemyState(Mushdoom* enemy) : EnemyState(enemy)
{
	name = "Attack";
}

void AttackEnemyState::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Attack"))
	{
		enemy->animation->ActiveAnimation("attack");
	}

	//enemy->is_attacking = true;
}

void AttackEnemyState::OnStateUpdate()
{
	//if (!enemy->PlayerInRange())
	//{
	//	if (enemy->animation->GetCurrentClipPercentatge() >= 0.95f)
	//	{
	//		Exit(enemy->pursue_state);
	//	}
	//}

	if (enemy->animation->GetCurrentClipPercentatge() >= 0.95f)
	{
		Exit(enemy->pursue_state);
	}
}

void AttackEnemyState::OnStateExit()
{
	enemy->is_attacking = false;
}
