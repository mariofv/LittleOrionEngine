#include "PursueEnemyState.h"

#include "Mushdoom.h"

#include "Component/ComponentAnimation.h"

#include "Main/GameObject.h"

PursueEnemyState* PursueEnemyStateDLL()
{
	PursueEnemyState* instance = new PursueEnemyState();
	return instance;
}

PursueEnemyState::PursueEnemyState()
{
	panel = new PanelComponent();
}

PursueEnemyState::PursueEnemyState(Mushdoom* enemy) : EnemyState(enemy)
{
	name = "Pursue";
}

void PursueEnemyState::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Pursue"))
	{
		enemy->animation->ActiveAnimation("pursue");
	}
}

void PursueEnemyState::OnStateUpdate()
{
	//if (enemy->SlotsAvailable())
	//{
	//	if (!enemy->PlayerInRange())
	//	{
	//		enemy->SeekPlayer();
	//	}
	//	else
	//	{
	//		Exit(enemy->attack_state);
	//	}
	//}
	//else
	//{
	//	if (!enemy->animation->IsOnState("Idle"))
	//	{
	//		enemy->animation->ActiveAnimation("idle");
	//	}

	//	Exit(enemy->idle_state);
	//}
	enemy->BattleCircleAI();

	if (enemy->is_attacking)
	{
		Exit(enemy->attack_state);
	}
}

void PursueEnemyState::OnStateExit()
{
}
