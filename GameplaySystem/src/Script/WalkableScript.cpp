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
	ImGui::DragFloat("Speed", &speed, 0.01f, 0.f, 2.5f);
	ImGui::DragFloat("Rotation Speed", &rotation_speed, 0.01f, 0.f, 0.5f);
}

void WalkableScript::Move()
{
	//example how to get variables from the engine
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();

	//float2 axis = App->input->GetAxisContoller(ControllerAxis::LEFT_JOYSTICK);

	//float3 target_pos = float3(transform.x + speed * axis.x, transform.y, transform.z + speed * axis.y);
	//if (App->artificial_intelligence->IsPointWalkable(target_pos))
	//owner->transform.SetTranslation(target_pos);

	float3 new_transform = transform;

	//EXAMPLE USING PLAYER INPUT
	if (App->input->GetKey(KeyCode::A))
	{
		new_transform += float3(speed, 0, 0);
	}
	if (App->input->GetKey(KeyCode::W))
	{
		new_transform += float3(0, 0, speed);
	}
	if (App->input->GetKey(KeyCode::S))
	{
		new_transform += float3(0, 0, -speed);
	}
	if (App->input->GetKey(KeyCode::D))
	{
		new_transform += float3(-speed, 0, 0);
	}
	if (App->input->GetKey(KeyCode::E))
	{
		owner->transform.SetRotation(float3(rotation.x, rotation.y - rotation_speed, rotation.z));
	}
	if (App->input->GetKey(KeyCode::Q))
	{
		owner->transform.SetRotation(float3(rotation.x, rotation.y + rotation_speed, rotation.z));
	}

	if (App->artificial_intelligence->IsPointWalkable(new_transform))
		owner->transform.SetTranslation(new_transform);
}
