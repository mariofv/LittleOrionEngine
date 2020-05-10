#include "ScreamEnemyState.h"

#include "Mushdoom.h"

#include "Component/ComponentAnimation.h"

ScreamEnemyState* ScreamEnemyStateDLL()
{
	ScreamEnemyState* instance = new ScreamEnemyState();
	return instance;
}

ScreamEnemyState::ScreamEnemyState()
{
	panel = new PanelComponent();
}

ScreamEnemyState::ScreamEnemyState(Mushdoom* enemy) : EnemyState(enemy)
{
	name = "Scream";
}

void ScreamEnemyState::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Scream"))
	{
		enemy->animation->ActiveAnimation("scream");
	}
}

void ScreamEnemyState::OnStateUpdate()
{
	if (enemy->animation->GetCurrentClipPercentatge() > 0.99f)
	{
		Exit(enemy->pursue_state);
	}
}

void ScreamEnemyState::OnStateExit()
{
}
