#include "ExampleScript.h"
#include "Main/Application.h"
#include "Module/ModuleInput.h"
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
	if (App->input->GetKeyDown(KeyCode::A)) {
		++i;
	}

}

void ExampleScript::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Text("Testing Inspector");
	ImGui::Text("By Marco and Jordi guys, we are doing it, yeah!");
	ImGui::Text("%d", i);
	ImGui::Text("Se que es cutre Mario, pero es para testear");
	
}

void ExampleScript::Test()
{
	//TODO
}
