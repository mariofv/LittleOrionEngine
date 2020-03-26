#include "Player2InputScript.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



Player2InputScript* Player2InputScriptDLL()
{
	Player2InputScript *instance = new Player2InputScript();
	return instance;
}

Player2InputScript::Player2InputScript()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void Player2InputScript::Awake()
{

}

// Use this for initialization
void Player2InputScript::Start()
{

}

// Update is called once per frame
void Player2InputScript::Update()
{
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();

	if (App->input->GetControllerButton(ControllerCode::A, PlayerID::TWO) || App->input->GetAxisController(ControllerAxis::LEFT_JOYSTICK, PlayerID::TWO).y != 0)
	{
		owner->transform.SetTranslation(float3(transform.x + speed, transform.y, transform.z));
	}
	if (App->input->GetControllerButton(ControllerCode::B, PlayerID::TWO) || App->input->GetTriggerController(ControllerAxis::LEFT_TRIGGER, PlayerID::TWO) != 0)
	{
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z + speed));
	}
	if (App->input->GetControllerButton(ControllerCode::X, PlayerID::TWO))
	{
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z - speed));
	}
	if (App->input->GetControllerButton(ControllerCode::Y, PlayerID::TWO))
	{
		owner->transform.SetTranslation(float3(transform.x - speed, transform.y, transform.z));
	}
}

// Use this for showing variables on inspector
void Player2InputScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);

}
