#include "PlayerMovement.h"

#include "Component/ComponentCamera.h"
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
	game_camera = (ComponentCamera*)camera->GetComponent(Component::ComponentType::CAMERA);
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
	ImGui::Text("Variables: ");
	ShowDraggedObjects();
	ImGui::Checkbox("Is Inside Frustum", &is_inside);

}

void PlayerMovement::Move(int player_id)
{
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();

	//Controller Input
	//float2 axis = App->input->GetAxisControllerRaw(ControllerAxis::LEFT_JOYSTICK_RAW, static_cast<ControllerID>(player_id));
	//float3 axis_direction = float3(axis.x, 0.0f, axis.y);

	//float vertical = App->input->GetVertical(static_cast<PlayerID>(player_id));
	//float horizontal = App->input->GetHorizontal(static_cast<PlayerID>(player_id));
	//float3 axis_direction = float3(horizontal, 0.0f, vertical);

	//if (!axis_direction.Equals(float3::zero))
	//{
	//	float3 dir;
	//	float3 direction = axis_direction * speed + transform;
	//	owner->transform.LookAt(direction);
	//	collider->AddForce(direction);
	//}

	//Keyboard Input
	float3 new_transform = float3::zero;
	if(player_id == 0)
	{
		new_transform = float3::zero;
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
			new_transform += float3(0.0f, jump_power, 0.0f);
			collider->AddForce(new_transform);
		}

		is_inside = IsInside(transform + new_transform/10);

		if (is_inside)
		{
			collider->AddForce(new_transform);
		}
		else
		{
			float3 null = float3(0.f, 0.f, 0.f);
			collider->SetVelocity(null);
		}
	}
	else if(player_id == 1)
	{
		new_transform = float3::zero;
		if (App->input->GetKey(KeyCode::LeftArrow))
		{
			new_transform += float3(speed, 0.0f, 0.0f);
		}
		if (App->input->GetKey(KeyCode::UpArrow))
		{
			new_transform += float3(0.0f, 0.0f, -speed);
		}
		if (App->input->GetKey(KeyCode::DownArrow))
		{
			new_transform += float3(0.0f, 0.0f, speed);
		}
		if (App->input->GetKey(KeyCode::RightArrow))
		{
			new_transform += float3(-speed, 0.0f, 0.0f);
		}
		is_inside = IsInside(transform + new_transform / 10);

		if (is_inside)
		{
			collider->AddForce(new_transform);
		}
		else
		{
			float3 null = float3(0.f, 0.f, 0.f);
			collider->SetVelocity(null);
		}
	}
	//EXAMPLE USING PLAYER INPUT (JUST MOVE)
	/*	new_transform += float3(0.0F, jump_power, 0.0F);
		collider->AddForce(new_transform);
	}
	if (new_transform.x != 0 || new_transform.y != 0 || new_transform.z != 0)
	{
		collider->SetVelocity(new_transform);
	}*/
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

bool PlayerMovement::IsInside(float3 future_transform)
{
	float3 distance = future_transform - owner->transform.GetTranslation();
	AABB future_position = AABB(owner->aabb.bounding_box.minPoint + distance, owner->aabb.bounding_box.maxPoint + distance);

	return game_camera->IsInsideFrustum(future_position);
}


void PlayerMovement::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&camera);

	variable_names.push_back(GET_VARIABLE_NAME(camera));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
