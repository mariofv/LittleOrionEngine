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

}

void PlayerMovement::Move(int player)
{

	float3 transform = owner->transform.GetTranslation();
	new_translation = float3::zero;
	PlayerID player_id = static_cast<PlayerID>(player - 1);

	float x_axis = App->input->GetHorizontal(player_id);
	float y_axis = App->input->GetVertical(player_id);


	new_translation += float3(x_axis, 0.0f, y_axis);
	if(IsGrounded())
	{
		is_grounded = true;
		if (!new_translation.Equals(float3::zero))
		{
			is_inside = IsInside(transform + new_translation*speed/10);

			if (is_inside)
			{
				collider->SetVelocity(new_translation, speed);
			}
			else
			{
				collider->SetVelocity(transform, 0);
			}
		}
		if (App->input->GetKey(KeyCode::Space))
		{
			new_translation += float3(0.0f, jump_power, 0.0f);
			collider->AddForce(new_translation);
		}

	}
	else 
	{
		is_grounded = false;
		if (!new_translation.Equals(float3::zero))
		{
			is_inside = IsInside(transform + new_translation * speed / 10);

			if (is_inside)
			{
				new_translation *= speed/2;
				collider->AddForce(new_translation);
			}
			else
			{
				collider->SetVelocity(transform, 0);
			}
		}
	}
}

void PlayerMovement::Jump()
{	
	float3 new_transform = float3::zero;
	new_transform += float3(0.0f, jump_power, 0.0f);
	collider->AddForce(new_transform);
}

bool PlayerMovement::IsGrounded()
{
	btVector3 origin = collider->body->getWorldTransform().getOrigin();

	btVector3 end = collider->body->getWorldTransform().getOrigin();
	end.setY(end.getY() - 1.05 * collider->box_size.getY());

	return collider->RaycastHit(origin,end);
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
