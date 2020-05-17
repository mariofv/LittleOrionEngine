#include "DieEnemyState.h"

#include "Mushdoom.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Component/ComponentCollider.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

DieEnemyState* DieEnemyStateDLL()
{
	DieEnemyState* instance = new DieEnemyState();
	return instance;
}

DieEnemyState::DieEnemyState()
{
	panel = new PanelComponent();
}

DieEnemyState::DieEnemyState(Mushdoom* enemy) : EnemyState(enemy)
{
	name = "Die";
}

void DieEnemyState::OnStateEnter()
{
	if (!enemy->animation->IsOnState("Die"))
	{
		enemy->animation->ActiveAnimation("die");
	}
}

void DieEnemyState::OnStateUpdate()
{
	if (enemy->animation->IsOnState("Die") && enemy->animation->GetCurrentClipPercentatge() >= 0.95f)
	{
		enemy->collider->detect_collision = false;
		enemy->collider->active_physics = false;

		enemy->collider->Disable();
		enemy->collider->SetCollisionDetection();

		enemy->activate_timer = true;
	}
}

void DieEnemyState::OnStateExit()
{
	enemy->is_alive = false;
	enemy->enemy_manager->KillEnemy(enemy);
}
