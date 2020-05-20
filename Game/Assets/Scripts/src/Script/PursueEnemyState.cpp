#include "PursueEnemyState.h"

#include "Mushdoom.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentAudioSource.h"

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

	//Animation has 60 frames, steps each 15 frames (1,15,30,45)
	float current_percentage = enemy->animation->GetCurrentClipPercentatge();
	if ((int)(current_percentage * 100) % 25 == 0 && current_percentage != 1)
	{
		enemy->audio_source->PlayEvent("play_footstep");
	}
}

void PursueEnemyState::OnStateExit()
{
}
