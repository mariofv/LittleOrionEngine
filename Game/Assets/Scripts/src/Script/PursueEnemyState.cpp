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
	if (!enemy->PlayerInRange())
	{
		enemy->SeekPlayer();
	}
	else
	{
		Exit(enemy->attack_state);
	}
}

void PursueEnemyState::OnStateExit()
{
}
