#include "AttackEnemyState.h"

#include "Mushdoom.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"

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
		enemy->audio_source->PlayEvent("play_make_attack");
	}

	enemy->is_attacking = true;
	attacked = true;
}

void AttackEnemyState::OnStateUpdate()
{
	if (enemy->animation->GetCurrentClipPercentatge() >= 0.40f && enemy->animation->GetCurrentClipPercentatge() <= 0.60f && enemy->PlayerInRange() && attacked)
	{
		enemy->audio_source->PlayEvent("play_attack_hits");
		enemy->Attack();
		attacked = false;
	}

	if (enemy->animation->GetCurrentClipPercentatge() >= 0.95f)
	{
		Exit(enemy->pursue_state);
	}

}

void AttackEnemyState::OnStateExit()
{
	enemy->is_attacking = false;
	enemy->CancelAttack();
	enemy->GetOutOfAttackRange();
}
