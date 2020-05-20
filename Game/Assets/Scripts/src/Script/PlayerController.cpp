#include "PlayerController.h"

#include "Component/ComponentCollider.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "PlayerAttack.h"
#include "PlayerMovement.h"
#include "UIManager.h"

PlayerController* PlayerControllerDLL()
{
	PlayerController* instance = new PlayerController();
	return instance;
}

PlayerController::PlayerController()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void PlayerController::Awake()
{
	const ComponentScript* component = owner->GetComponentScript("PlayerMovement");
	player_movement = static_cast<PlayerMovement*>(component->script);

	const ComponentScript* component_attack = owner->GetComponentScript("PlayerAttack");
	player_attack = static_cast<PlayerAttack*>(component_attack->script);

	GameObject* ui = App->scene->GetGameObjectByName("UIManager");
	ComponentScript* component_ui = ui->GetComponentScript("UIManager");
	ui_manager = static_cast<UIManager*>(component_ui->script);

}

// Use this for initialization
void PlayerController::Start()
{

}

// Update is called once per frame
void PlayerController::Update()
{
	bool is_attacking = player_attack->Attack(player);

	if(!is_attacking)
	{
		player_movement->Move(player);
	}

}

// Use this for showing variables on inspector
void PlayerController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ImGui::Text("Player Controller Script Inspector");
	ShowDraggedObjects();
	std::string selected = std::to_string(player);
	if (ImGui::BeginCombo("Player", selected.c_str()))
	{
		
		if (ImGui::Selectable("1"))
		{
			player = 1;
		}
		if (ImGui::Selectable("2"))
		{
			player = 2;
		}

		ImGui::EndCombo();
	}
	ImGui::DragFloat("Health", &health_points);
	ImGui::Checkbox("Invincible", &invincible);
	ImGui::Checkbox("Is Alive", &is_alive);
	
}


//Use this for linking GO automatically
void PlayerController::Save(Config& config) const
{
	config.AddUInt(player, "Player");
	Script::Save(config);
}

//Use this for linking GO automatically
void PlayerController::Load(const Config& config)
{
	player = static_cast<unsigned>(config.GetUInt("Player", player));
	Script::Load(config);
}

void PlayerController::TakeDamage(float damage)
{ 
	if (invincible) return;

	health_points -= damage;
	if(player == 1)
	{
		ui_manager->SetPlayer1Health(health_points / total_health);
	}
	else
	{
		ui_manager->SetPlayer2Health(health_points / total_health);
	}

	if (health_points <= 0)
	{
		health_points = 0;
		is_alive = false;
		owner->SetEnabled(false);
	}
}

ComponentCollider* PlayerController::GetCollider()
{
	return static_cast<ComponentCollider*>(owner->GetComponent(Component::ComponentType::COLLIDER));
}

void PlayerController::MakePlayerFall(float3& direction) const
{
	player_movement->collider->ClearForces();
	//player_movement->Jump(direction);
}

