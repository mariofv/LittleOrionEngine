#include "PlayerController.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "PlayerMovement.h"

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
	std::string aux("PlayerMovement");
	ComponentScript* component = owner->GetComponentScript(aux);
	player_movement = (PlayerMovement*)component->script;
}

// Use this for initialization
void PlayerController::Start()
{

}

// Update is called once per frame
void PlayerController::Update()
{
	player_movement->Move(player);
}

// Use this for showing variables on inspector
void PlayerController::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
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

}

//Use this for linking GO automatically
void PlayerController::Save(Config& config) const
{
	config.AddUInt(player, "Player");
}

//Use this for linking GO automatically
void PlayerController::Load(const Config& config)
{
	player = config.GetUInt("Player", player);
}

//Use this for linking GO automatically
void PlayerController::Link()
{
	//example = App->scene->GetGameObject(exampleUUID);
}
