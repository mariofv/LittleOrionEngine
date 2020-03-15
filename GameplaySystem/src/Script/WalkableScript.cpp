#include "WalkableScript.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAI.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



WalkableScript* WalkableScriptDLL()
{
	WalkableScript *instance = new WalkableScript();
	return instance;
}

WalkableScript::WalkableScript()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void WalkableScript::Awake()
{

}

// Use this for initialization
void WalkableScript::Start()
{

}

// Update is called once per frame
void WalkableScript::Update()
{
	Move();
}

// Use this for showing variables on inspector
void WalkableScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	//Example to show text
	ImGui::Text("Example Script Inspector");
	//ImGui::Text(GET_VARIABLE_NAME(speed));
	//Example Showing variables and being able to modify it on Runtime.
	ImGui::DragFloat("Speed", &speed, 0.01f, 0.f, 0.5f);
	ImGui::DragFloat("Rotation Speed", &rotation_speed, 0.01f, 0.f, 0.5f);
}

void WalkableScript::Move()
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
