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
	ImGui::Text("Example Script Inspector");
	ImGui::DragFloat("Speed", &speed,0.01f,0.f,0.5f);
	ImGui::DragFloat("Rotation Speed", &rotation_speed, 0.01f, 0.f, 0.5f);
	ImGui::Text("Test equsals");
	
}

void ExampleScript::Test()
{
	float3 transform = owner->transform.GetTranslation();
	float3 rotation = owner->transform.GetRotationRadiants();
	//TODO
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
		owner->transform.SetRotation(float3(rotation.x, rotation.y + rotation_speed, rotation.z));
	}
}
