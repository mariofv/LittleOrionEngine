#include "Player1InputScript.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"



Player1InputScript* Player1InputScriptDLL()
{
	Player1InputScript *instance = new Player1InputScript();
	return instance;
}

Player1InputScript::Player1InputScript()
{
	panel = new PanelComponent();
}

// Use this for initialization before Start()
void Player1InputScript::Awake()
{

}

// Use this for initialization
void Player1InputScript::Start()
{

}

// Update is called once per frame
void Player1InputScript::Update()
{
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();

	if (App->input->GetControllerButton(ControllerCode::A, PlayerID::ONE) || App->input->GetAxisControllerRaw(ControllerAxis::LEFT_JOYSTICK_RAW, PlayerID::TWO).y != 0)
	{
		owner->transform.SetTranslation(float3(transform.x + speed, transform.y, transform.z));
	}
	if (App->input->GetControllerButton(ControllerCode::B, PlayerID::ONE) || App->input->GetTriggerController(ControllerAxis::LEFT_TRIGGER, PlayerID::ONE) != 0)
	{
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z + speed));
	}
	if (App->input->GetControllerButton(ControllerCode::X, PlayerID::ONE))
	{
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z - speed));
	}
	if (App->input->GetControllerButton(ControllerCode::Y, PlayerID::ONE))
	{
		owner->transform.SetTranslation(float3(transform.x - speed, transform.y, transform.z));
	}
}

// Use this for showing variables on inspector
void Player1InputScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);

}
