#include "PlayerAttack.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "EnemyController.h"


PlayerAttack* PlayerAttackDLL()
{
	PlayerAttack* instance = new PlayerAttack();

	return instance;
}



PlayerAttack::PlayerAttack()
{
	panel = new PanelComponent();
}
// Use this for initialization before Start()

void PlayerAttack::Awake()
{
	GameObject* enemy_manager_go = App->scene->GetGameObjectByName("EnemyManager");
	ComponentScript* enemy_manager_component = enemy_manager_go->GetComponentScript("EnemyManager");
	enemy_manager = static_cast<EnemyManager*>(enemy_manager_component->script);

	animation = (ComponentAnimation*) owner->GetComponent(Component::ComponentType::ANIMATION);
	
	component_collider = static_cast<ComponentCollider*>(collider->GetComponent(ComponentCollider::ColliderType::BOX));
	component_collider->DisablePhysics(false);
	collider->SetEnabled(false);
}
// Use this for initialization

void PlayerAttack::Start()
{

}
// Update is called once per frame

bool PlayerAttack::Attack()
{
	is_attacking = animation->IsOnState("Punch") || animation->IsOnState("Kick");

	if(!is_attacking && !collider->IsEnabled())
	{
		if(App->input->GetGameInputDown("Punch", PlayerID::ONE))
		{
			animation->ActiveAnimation("punch");
			//Active colliders of hands
			collider->SetEnabled(true);
			current_damage_power = PUNCH_DAMAGE;
		}
		else if(App->input->GetGameInputDown("Kick", PlayerID::ONE))
		{
			animation->ActiveAnimation("kick");
			//Active colliders of kick
			collider->SetEnabled(true);
			current_damage_power = KICK_DAMAGE;
		}	
	}

	if (collider->IsEnabled() && animation->GetCurrentClipPercentatge() > 0.5f)
	{
		ComputeCollisions();
		collider->SetEnabled(false);		
	}

	return is_attacking;
}
void PlayerAttack::ComputeCollisions() const
{
	for (auto& enemy : enemy_manager->enemies)
	{
		if (!enemy->is_alive)
		{
			continue;
		}

		//if (collider->aabb.global_bounding_box.Intersects(enemy->owner->aabb.global_bounding_box))
		if (component_collider->DetectCollisionWith(enemy->collider_component))
		{
			enemy->TakeDamage(current_damage_power);
		}
	}
}
// Use this for showing variables on inspector

void PlayerAttack::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ShowDraggedObjects();
}
//Use this for linking JUST GO automatically 

void PlayerAttack::InitPublicGameObjects()
{
	public_gameobjects.push_back(&collider);
	variable_names.push_back(GET_VARIABLE_NAME(collider));

	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	for (int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}

}
