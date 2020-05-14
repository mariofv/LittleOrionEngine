#include "PlayerMovement.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentCollider.h"
#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAI.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleTime.h"
#include "Module/ModulePhysics.h"

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
	ImGui::Checkbox("Is Grounded", &is_grounded);
	ImGui::DragFloat3("VELOCITY", velocity.ptr(), 0.1f, 0.f, 300.f);
	ImGui::Checkbox("Future AABB", &visualize_future_aabb);
	ImGui::DragFloat3("Distance", distance.ptr(), 0.1f, 0.f, 300.f);
}

void PlayerMovement::Move(int player)
{
	
	velocity = collider->GetCurrentVelocity();
	float3 transform = owner->transform.GetTranslation();
	direction = float3::zero; //change direction
	PlayerID player_id = static_cast<PlayerID>(player - 1);

	float x_axis = App->input->GetHorizontal(player_id);
	float y_axis = App->input->GetVertical(player_id);

	if (abs(velocity.y) < 0.01 && is_jumping)
	{
		is_jumping = false;
	}

	direction = float3(x_axis, 0.0f, y_axis); // not add just assing
	if (IsGrounded() && !is_jumping)
	{
		is_grounded = true;
		if (!direction.Equals(float3::zero))
		{
			is_inside = IsInside(transform + direction  * speed);
			if (is_inside)
			{
				collider->SetVelocity(direction, speed * App->time->delta_time);
			}
			else
			{
				direction = float3::zero;
				collider->AddForce(direction);
				collider->SetVelocity(direction, 0);
			}
		}
		if (App->input->GetKeyDown(KeyCode::Space))
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
			is_inside = IsInside(transform + direction * speed / 10);

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
	end.setY(end.getY() - 1.7f);

	return collider->RaycastHit(origin,end);
}

bool PlayerMovement::IsInside(float3 future_transform)
{
	distance = (future_transform) - owner->transform.GetTranslation();
	//Harcoded distance in order to camera works
	distance.x = distance.x * 10.f;
	distance.y = distance.y * 10.f;
	distance.z = distance.z * 10.f;
	AABB future_position = AABB(owner->aabb.global_bounding_box.minPoint + distance, owner->aabb.global_bounding_box.maxPoint + distance);
	if(visualize_future_aabb)
	{
		App->debug_draw->RenderSingleAABB(future_position);
	}

	if(second_player != nullptr && !App->input->singleplayer_input)
	{
		float3 go_distance = second_player->transform.GetTranslation() - future_transform;
		return game_camera->IsInsideFrustum(future_position) && go_distance.x < 18;
	}

	return game_camera->IsInsideFrustum(future_position);
}


void PlayerMovement::InitPublicGameObjects()
{
	//IMPORTANT, public gameobjects, name_gameobjects and go_uuids MUST have same size

	public_gameobjects.push_back(&camera);
	public_gameobjects.push_back(&second_player);

	variable_names.push_back(GET_VARIABLE_NAME(camera));
	variable_names.push_back(GET_VARIABLE_NAME(second_player));

	for (unsigned int i = 0; i < public_gameobjects.size(); ++i)
	{
		name_gameobjects.push_back(is_object);
		go_uuids.push_back(0);
	}
}
