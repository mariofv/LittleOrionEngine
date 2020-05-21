#include "PlayerMovement.h"

#include "Component/ComponentAnimation.h"
#include "Component/ComponentCamera.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAI.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"
#include "Module/ModulePhysics.h"

#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "WorldManager.h"


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
	animation = static_cast<ComponentAnimation*>(owner->GetComponent(Component::ComponentType::ANIMATION));
}

// Use this for initialization
void PlayerMovement::Start()
{
	GameObject* world_go = App->scene->GetGameObjectByName("World Manager");
	ComponentScript* world_component = world_go->GetComponentScript("WorldManager");
	world = static_cast<WorldManager*>(world_component->script);
}

// Use this for showing variables on inspector
void PlayerMovement::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ImGui::Text("Player Movement Script Inspector");
	//Example Showing variables and being able to modify it on Runtime.
	ImGui::DragFloat("Speed", &speed, 0.1f, 0.f, 0.5f);
	ImGui::DragFloat("Jump Power", &jump_power, 2.0f, 2.0f, 10.0f);
	ImGui::Text("Variables: ");
	ShowDraggedObjects();
	ImGui::Checkbox("Is Inside Frustum", &is_inside);
	ImGui::Checkbox("Is Jumping", &is_jumping);
	ImGui::Checkbox("Is Grounded", &is_grounded);
	ImGui::DragFloat3("Velocity", velocity.ptr(), 0.1f, 0.f, 300.f);
	ImGui::Checkbox("Future AABB", &visualize_future_aabb);
	ImGui::DragFloat3("Distance", distance.ptr(), 0.1f, 0.f, 300.f);
	ImGui::DragFloat3("Direction", direction.ptr(), 0.1f, 0.f, 300.f);
	ImGui::DragFloat3("Device Coordinates", device_coordinates.ptr());

	ImGui::Checkbox("SinglePlayer Input", &App->input->singleplayer_input);
}

void PlayerMovement::Move(int player)
{
	
	velocity = collider->GetCurrentVelocity();
	float3 transform = owner->transform.GetGlobalTranslation();
	direction = float3::zero; 
	PlayerID player_id = static_cast<PlayerID>(player - 1);

	float x_axis = App->input->GetHorizontal(player_id);
	float y_axis = App->input->GetVertical(player_id);

	if (abs(velocity.y) < 0.01 && is_jumping)
	{
		is_jumping = false;
		is_second_jump = false;
	}

	direction = float3(x_axis, 0.0f, y_axis);
	if (IsGrounded() && !is_jumping)
	{
		is_grounded = true;
		if (!direction.Equals(float3::zero))
		{
			is_inside = IsInside(transform + direction * speed);
			if (is_inside)
			{
				collider->SetVelocity(direction, speed * App->time->delta_time);
				animation->ActiveAnimation("run");
			}
			else
			{
				//direction = float3::zero;
				direction.x = direction.x * -speed;
				direction.z = direction.z * -speed;
				collider->AddForce(direction);
				collider->SetVelocity(direction, -speed);
			}
		}
		else
		{
			animation->ActiveAnimation("idle");
		}
		
		if (App->input->GetGameInputDown("Jump", player_id))
		{
			is_jumping = true;
			Jump(direction);
		}

	}
	else
	{
		is_grounded = false;
		if (!direction.Equals(float3::zero))
		{
			is_inside = IsInside(transform + direction * speed);

			if (is_inside)
			{
				direction *= App->time->delta_time * speed;
				collider->AddForce(direction);
			}
			else
			{
				direction = float3::zero;
				collider->AddForce(direction);
				collider->SetVelocity(transform, 0);
			}
		}
		if (App->input->GetGameInputDown("Jump", player_id) && !is_second_jump)
		{
			is_second_jump = true;
			direction.y = second_jump_factor;
			Jump(direction);
		}
	}
	
}

void PlayerMovement::Jump(float3& direction)
{
	direction += float3(0.0f, jump_power * App->time->delta_time, 0.0f);
	collider->AddForce(direction);
}

bool PlayerMovement::IsGrounded()
{
	btVector3 origin = collider->body->getWorldTransform().getOrigin();

	btVector3 end = collider->body->getWorldTransform().getOrigin();
	end.setY(end.getY() - collider->box_size.getY() * 1.5);

	return collider->RaycastHit(origin,end);
}

bool PlayerMovement::IsInside(float3 future_transform)
{
	bool inside = false;
	distance = (future_transform)-owner->transform.GetGlobalTranslation();
	float4 position_float4 = float4(future_transform, 1.f);
	float4 clip_coordinates = App->cameras->main_camera->GetClipMatrix() * position_float4;
	device_coordinates = clip_coordinates.xyz() / clip_coordinates.w;

	if(device_coordinates.x < 1 && device_coordinates.x > -1 && device_coordinates.y < 1 && device_coordinates.y > -1)
	{
		inside = true;
	}
	if(other_player != nullptr && !world->singleplayer)
	{
		float4 position_float4_other_player = float4(other_player->transform.GetGlobalTranslation() - distance, 1.f);
		float4 clip_coordinates_other_player = game_camera->GetClipMatrix() * position_float4_other_player;
		float3 device_coordinates_other_player = clip_coordinates_other_player.xyz() / clip_coordinates_other_player.w;

		if (device_coordinates_other_player.x > 1 || device_coordinates_other_player.x < -1 
			|| device_coordinates_other_player.y > 1 || device_coordinates_other_player.y < -1)
		{
			inside = false;
		}
	}
	return inside;
}

void PlayerMovement::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&camera);
	public_gameobjects.push_back(&other_player);

	variable_names.push_back(GET_VARIABLE_NAME(camera));
	variable_names.push_back(GET_VARIABLE_NAME(other_player));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
