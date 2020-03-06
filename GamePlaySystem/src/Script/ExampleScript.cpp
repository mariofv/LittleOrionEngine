#include "ExampleScript.h"
#include "Main/Application.h"
#include "Module/ModuleInput.h"
#include "Main/GameObject.h"
#include "Component/ComponentTransform.h"
#include "Module/ModuleScene.h"
#include "imgui.h"


ExampleScript* ExampleScriptDLL()
{
	ExampleScript *instance = new ExampleScript();
	return instance;
}

ExampleScript::ExampleScript()
{
}
ExampleScript::~ExampleScript()
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
	ImGui::SetCurrentContext(context);
	ImGui::Text("Testing Inspector");
	ImGui::Text("By Marco and Jordi guys, we are doing it, yeah!");
	ImGui::DragFloat("Speed", &speed,0.01f,0.f,0.5f);
	ImGui::Text("Test 23232");
	
}

void ExampleScript::Test()
{
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();
	//TODO
	if (App->input->GetKey(KeyCode::A))
	{
		++i;
		owner->transform.SetTranslation(float3(transform.x + speed, transform.y, transform.z));
	}
	if (App->input->GetKey(KeyCode::W))
	{
		++i;
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z + speed));
	}
	if (App->input->GetKey(KeyCode::S))
	{
		++i;
		owner->transform.SetTranslation(float3(transform.x, transform.y, transform.z - speed));
	}
	if (App->input->GetKey(KeyCode::D))
	{
		++i;
		owner->transform.SetTranslation(float3(transform.x - speed, transform.y, transform.z));
	}
	if (App->input->GetKey(KeyCode::E))
	{
		++i;
		owner->transform.SetRotation(float3(rotation.x, rotation.y - speed, rotation.z));
	}
	if (App->input->GetKey(KeyCode::Q))
	{
		++i;
		owner->transform.SetRotation(float3(rotation.x, rotation.y + speed, rotation.z));
	}
}
