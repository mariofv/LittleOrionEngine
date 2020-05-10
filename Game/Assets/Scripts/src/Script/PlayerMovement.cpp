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

void PlayerMovement::Move(int player_id)
{
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();

	//Controller Input
	//float2 axis = App->input->GetAxisControllerRaw(ControllerAxis::LEFT_JOYSTICK_RAW, static_cast<ControllerID>(player_id));
	//float3 axis_direction = float3(axis.x, 0.0f, axis.y);

	float vertical = App->input->GetVertical(static_cast<PlayerID>(player_id));
	float horizontal = App->input->GetHorizontal(static_cast<PlayerID>(player_id));
	float3 axis_direction = float3(horizontal, 0.0f, vertical);

	if (!axis_direction.Equals(float3::zero))
	{
		float3 dir;
		float3 direction = axis_direction * speed + transform;
		bool there_is_poly = App->artificial_intelligence->FindNextPolyByDirection(direction, dir);
		
		if(there_is_poly)
		{
			direction.y = dir.y;
		}

		if (App->artificial_intelligence->IsPointWalkable(direction))
		{
		
			owner->transform.LookAt(direction);
			owner->transform.SetTranslation(direction);
		}
	}

	//Keyboard Input
	float3 new_transform = float3(0.0F, 0.0F, 0.0F);

	//EXAMPLE USING PLAYER INPUT (JUST MOVE)
	if (App->input->GetKey(KeyCode::D))
	{
		new_transform += float3(speed, 0.0f, 0.0f);
	}
	if (App->input->GetKey(KeyCode::W))
	{
		new_transform += float3(0.0f, 0.0f, -speed);
	}
	if (App->input->GetKey(KeyCode::S))
	{
		new_transform += float3(0.0f, 0.0f, speed);
	}
	if (App->input->GetKey(KeyCode::A))
	{
		new_transform += float3(-speed, 0.0f, 0.0f);
	}
	if (App->input->GetKeyDown(KeyCode::Space))
	{
		new_transform += float3(0.0F, jump_power, 0.0F);
		collider->AddForce(new_transform);
	}
	
	collider->SetVelocity(new_transform, speed);
	
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
