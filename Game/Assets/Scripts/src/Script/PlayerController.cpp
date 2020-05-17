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
#include "DebugModeScript.h"

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
	player_movement = (PlayerMovement*)component->script;

	const ComponentScript* component_attack = owner->GetComponentScript("PlayerAttack");
	player_attack = (PlayerAttack*)component_attack->script;

	const ComponentScript* component_debug = owner->GetComponentScript("DebugModeScript");
	debug = (DebugModeScript*)component_debug->script;
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
	ShowDraggedObjects();
	ImGui::Text("Player Controller Script Inspector");
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
}

//Use this for linking GO automatically
void PlayerController::Save(Config& config) const
{
	config.AddUInt(player, "Player");
}

//Use this for linking GO automatically
void PlayerController::Load(const Config& config)
{
	player = static_cast<unsigned>(config.GetUInt("Player", player));
}

void PlayerController::TakeDamage(float damage)
{
	if (debug->is_player_invincible) return;

	health_points -= damage;
	//UPDATE HEALTH_BAR HERE
	//also addforce here
}

ComponentCollider* PlayerController::GetCollider()
{
	return static_cast<ComponentCollider*>(owner->GetComponent(Component::ComponentType::COLLIDER));
}

//Use this for linking JUST GO automatically 
void PlayerController::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size
	public_gameobjects.push_back(&debug_system);
	variable_names.push_back(GET_VARIABLE_NAME(debug_system));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}

