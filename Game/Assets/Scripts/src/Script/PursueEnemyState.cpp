#include "PursueEnemyState.h"

#include "Mushdoom.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentTransform.h"

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
		const float3 player_translation = enemy->player->transform.GetTranslation();
		float3 translation = enemy->owner->transform.GetTranslation();

		float3 direction = player_translation - translation;
		float3 position = translation + (direction.Normalized() * enemy->GetMoveSpeed());

		float3 next_position;
		if (enemy->PointInNavMesh(position, next_position))
		{
			position.y = next_position.y;
		}

		if (enemy->IsPointWalkable(position))
		{
			enemy->LookAndMoveToPoint(position);
		}
	}
	else
	{
		Exit(enemy->attack_state);
	}
}

void PursueEnemyState::OnStateExit()
{
}
