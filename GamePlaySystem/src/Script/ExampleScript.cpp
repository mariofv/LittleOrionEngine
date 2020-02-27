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
	float3 transform = owner->transform.GetTranslation();
	
	if (App->input->GetKey(KeyCode::A)) 
	{
		++i;
		owner->transform.SetTranslation(float3(transform.x + speed,transform.y,transform.z));
	}
	if (App->input->GetKeyDown(KeyCode::S)) 
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
	ImGui::Text("Se que es cutre Mario, pero es para testear");
	
}

void ExampleScript::Test()
{
	//TODO
}
