#include "TestScriptRuntime.h"

#include "Component/ComponentTransform.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleScene.h"

#include "imgui.h"

TestScriptRuntime* TestScriptRuntimeDLL()
{
	TestScriptRuntime *instance = new TestScriptRuntime();
	return instance;
}

TestScriptRuntime::TestScriptRuntime()
{
}

void TestScriptRuntime::Update()
{

	//Test();

	/*if (App->input->GetKeyDown(KeyCode::C))
	{
		GameObject* go = App->scene->CreateGameObject();
	}*/
}

void TestScriptRuntime::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Text("TestingScript on Runtime");
	ImGui::DragFloat("Speed", &speed, 0.01f, 0.f, 0.5f);
	ImGui::DragFloat("Rotation Speed", &rotation_speed, 0.01f, 0.f, 0.5f);
	ImGui::Text("Testing for QA: Attempts 94.949494");

}

void TestScriptRuntime::Test()
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
