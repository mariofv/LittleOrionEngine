#include "PlayerMovement.h"

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
	ImGui::DragFloat("Speed", &speed, 0.2f, 0.f, 0.5f);
	ImGui::DragFloat("Rotation Speed", &rotation_speed, 0.01f, 0.f, 0.5f);
	ImGui::DragFloat("Jump Power", &jump_power, 2.0f, 2.0f, 10.0f);
}

void PlayerMovement::Move(int player_id)
{
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();

	//Controller Input
	float2 axis = App->input->GetAxisControllerRaw(ControllerAxis::LEFT_JOYSTICK_RAW, static_cast<PlayerID>(player_id));
	float3 axis_direction = float3(-axis.x, 0.0f, -axis.y);

	//if(on_ramp)
	//{
	//	float heigth = owner->transform.GetTranslation().y;
	//	if (App->input->GetKey(KeyCode::A) || axis.x < 0)
	//	{
	//		heigth = sin(math::DegToRad(15.0f));
	//	}
	//	else if((App->input->GetKey(KeyCode::D)) || axis.x > 0)
	//	{
	//		heigth = -sin(math::DegToRad(15.0f));
	//	}
	//	axis_direction.y += heigth;
	//}

	if (!axis_direction.Equals(float3::zero))
	{
		float3 dir;
		float3 direction = axis_direction * speed + transform;
		App->artificial_intelligence->FindNextPolyByDirection(direction, dir);
		direction.y = dir.y;
		owner->transform.LookAt(direction);

		if (App->artificial_intelligence->IsPointWalkable(direction))
			owner->transform.SetTranslation(direction);
	}

	//Keyboard Input
	float3 new_transform = transform;

	//EXAMPLE USING PLAYER INPUT (JUST MOVE)
	if (App->input->GetKey(KeyCode::A))
	{
		new_transform += float3(speed, 0.0f, 0.0f);
	}
	if (App->input->GetKey(KeyCode::W))
	{
		new_transform += float3(0.0f, 0.0f, speed);
	}
	if (App->input->GetKey(KeyCode::S))
	{
		new_transform += float3(0.0f, 0.0f, -speed);
	}
	if (App->input->GetKey(KeyCode::D))
	{
		new_transform += float3(-speed, 0.0f, 0.0f);
	}


	//Jump handle
	if (App->input->GetGameInput("Jump"))
	{
		if (!is_jumping)
		{
			is_jumping = true;
			current_y = transform.y;
			movement_vector = float3(0.0f, jump_power, 0.0f);
		}
	}

	if (is_jumping)
	{
		//Delta time conversion to seconds
		new_transform += App->time->delta_time / 1000.0f * movement_vector;
		movement_vector += App->time->delta_time / 1000.0f * gravity_vector;

		if (new_transform.y <= current_y)
		{
			is_jumping = false;
			new_transform.y = current_y;
		}
	}

	if (!new_transform.Equals(transform))
	{
		float3 dir;
		App->artificial_intelligence->FindNextPolyByDirection(new_transform, dir);
		new_transform.y = dir.y;

		if (!is_jumping) 
		{
			owner->transform.LookAt(new_transform);
		}
		if (App->artificial_intelligence->IsPointWalkable(new_transform)) 
		{
			owner->transform.SetTranslation(new_transform);
		}
	}
}

void PlayerMovement::Dash()
{
	//TODO DASH
}
