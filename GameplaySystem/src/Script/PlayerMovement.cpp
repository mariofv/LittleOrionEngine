#include "PlayerMovement.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAI.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

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

}

// Update is called once per frame
//void PlayerMovement::Update()
//{
//
//
//}

// Use this for showing variables on inspector
void PlayerMovement::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	ImGui::Text("Player Movement Script Inspector");
	//Example Showing variables and being able to modify it on Runtime.
	ImGui::DragFloat("Speed", &speed, 0.5f, 0.f, 0.5f);
	ImGui::DragFloat("Rotation Speed", &rotation_speed, 0.01f, 0.f, 0.5f);
}

void PlayerMovement::Move()
{
	//example how to get variables from the engine
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();
	//EXAMPLE USING PLAYER INPUT
	if (App->input->GetKey(KeyCode::A))
	{
		float3 target_position = float3(transform.x + speed, transform.y, transform.z);

		if (App->artificial_intelligence->IsPointWalkable(target_position))
			owner->transform.SetTranslation(target_position);
	}
	if (App->input->GetKey(KeyCode::W))
	{
		float3 target_position = float3(transform.x, transform.y, transform.z + speed);

		if (App->artificial_intelligence->IsPointWalkable(target_position))
			owner->transform.SetTranslation(target_position);
	}
	if (App->input->GetKey(KeyCode::S))
	{
		float3 target_position = float3(transform.x, transform.y, transform.z - speed);

		if (App->artificial_intelligence->IsPointWalkable(target_position))
			owner->transform.SetTranslation(target_position);
	}
	if (App->input->GetKey(KeyCode::D))
	{
		float3 target_position = float3(transform.x - speed, transform.y, transform.z);

		if (App->artificial_intelligence->IsPointWalkable(target_position))
			owner->transform.SetTranslation(target_position);
	}
	if (App->input->GetKey(KeyCode::E))
	{
		owner->transform.SetRotation(float3(rotation.x, rotation.y - rotation_speed, rotation.z));
	}
	if (App->input->GetKey(KeyCode::Q))
	{
		owner->transform.SetRotation(float3(rotation.x, rotation.y + rotation_speed, rotation.z));
	}
}

//Use this for linking GO automatically
void PlayerMovement::Save(Config& config) const
{
	config.AddUInt(example->UUID, "ExampleNameforSave");
}

//Use this for linking GO automatically
void PlayerMovement::Load(const Config& config)
{
	exampleUUID = config.GetUInt("ExampleNameforSave", 0);
}

//Use this for linking GO automatically
void PlayerMovement::Link()
{
	example = App->scene->GetGameObject(exampleUUID);
}
