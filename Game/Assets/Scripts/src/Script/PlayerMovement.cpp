#include "PlayerMovement.h"

#include "Component/ComponentCollider.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAI.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"


PlayerMovement* PlayerMovementDLL()
{
	PlayerMovement* instance = new PlayerMovement();
	return instance;
}

PlayerMovement::PlayerMovement()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void PlayerMovement::Awake()
{

	collider = static_cast<ComponentCollider*>(owner->GetComponent(Component::ComponentType::COLLIDER));
}

// Use this for initialization
void PlayerMovement::Start()
{
	gravity_vector = float3(0.0f, -9.8f, 0.0f);
}


// Use this for showing variables on inspector
void PlayerMovement::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ImGui::Text("Player Movement Script Inspector");
	//Example Showing variables and being able to modify it on Runtime.
	ImGui::DragFloat("Speed", &speed, 0.1f, 0.f, 0.5f);
	ImGui::DragFloat("Rotation Speed", &rotation_speed, 0.01f, 0.f, 0.5f);
	ImGui::DragFloat("Jump Power", &jump_power, 2.0f, 2.0f, 10.0f);
}

void PlayerMovement::Move(int player)
{
	new_translation = float3::zero;
	PlayerID player_id = static_cast<PlayerID>(player - 1);

	float x_axis = App->input->GetHorizontal(player_id);
	float y_axis = App->input->GetVertical(player_id);

	new_translation += float3(x_axis, 0.0f, y_axis);

	if (!new_translation.Equals(float3::zero))
	{
		collider->SetVelocity(new_translation, speed);
	}

	if (App->input->GetGameInputDown("Punch", player_id))
	{
		new_translation += float3(0.0f, jump_power, 0.0f);
		collider->AddForce(new_translation);
	}

	if (App->input->GetKeyDown(KeyCode::Space))
	{
		new_translation += float3(0.0f, jump_power, 0.0f);
		collider->AddForce(new_translation);
	}
}

void PlayerMovement::Fall()
{
	float3 new_position = owner->transform.GetTranslation() + gravity_vector * falling_factor * App->time->delta_time / 1000.0f;
	falling_factor += 0.1f;
	owner->transform.SetTranslation(new_position);
}

void PlayerMovement::Dash()
{
	//TODO DASH
}
