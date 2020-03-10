#include "ExampleScript.h"

#include "Component/ComponentScript.h"
#include "Component/ComponentTransform.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "UI/Panel/InspectorSubpanel/PanelComponent.h"

#include "imgui.h"

#include "TestScriptRuntime.h"


ExampleScript* ExampleScriptDLL()
{
	ExampleScript *instance = new ExampleScript();
	return instance;
}

ExampleScript::ExampleScript()
{
	panel = new PanelComponent();
}

void ExampleScript::Awake()
{
}

void ExampleScript::Start()
{
}

void ExampleScript::Update()
{

	Test();

	if (App->input->GetKeyDown(KeyCode::C))
	{
		GameObject* go = App->scene->CreateGameObject();
	}
}

void ExampleScript::OnInspector(ImGuiContext* context)
{
	//Necessary to be able to write with imgui
	ImGui::SetCurrentContext(context);
	//Example to show text
	ImGui::Text("Example Script Inspector");
	//Example Showing variables and being able to modify it on Runtime.
	ImGui::DragFloat("Speed", &speed, 0.01f, 0.f, 0.5f);
	ImGui::DragFloat("Rotation Speed", &rotation_speed, 0.01f, 0.f, 0.5f);
	ImGui::Text("Testing for QA: attempt 24.84848484");
	//Example to Drag and drop and link GOs in the Editor, Unity-like (WIP)
	ImGui::Text("TestScriptRuntime: ");
	ImGui::SameLine();
	ImGui::Button(is_object.c_str());
	panel->DropGOTarget(enemy, "TestScriptRuntime", enemy_component);
	if (enemy)
	{
		//TODO this assign in Awake or Start functions
		enemy_script = (TestScriptRuntime*)enemy_component->script;
		is_object = enemy->name;
	}
}

void ExampleScript::Test()
{
	//example how to get variables from the engine
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();
	//EXAMPLE USING PLAYER INPUT
	if (App->input->GetKey(KeyCode::A))
	{
		owner->transform.SetTranslation(float3(transform.x + speed, transform.y, transform.z));
	}
	if (App->input->GetKey(KeyCode::W))
	{
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z + speed));
	}
	if (App->input->GetKey(KeyCode::S))
	{
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z - speed));
	}
	if (App->input->GetKey(KeyCode::D))
	{
		owner->transform.SetTranslation(float3(transform.x - speed, transform.y, transform.z));
	}
	if (App->input->GetKey(KeyCode::E))
	{
		owner->transform.SetRotation(float3(rotation.x, rotation.y - rotation_speed, rotation.z));
	}
	if (App->input->GetKey(KeyCode::Q))
	{
		owner->transform.SetRotation(float3(rotation.x, rotation.y + enemy_script->rotation_speed, rotation.z));
	}
	if (App->input->GetKey(KeyCode::T) && enemy)
	{
		enemy->transform.SetRotation(float3(rotation.x, rotation.y + enemy_script->rotation_speed, rotation.z));
	}
}