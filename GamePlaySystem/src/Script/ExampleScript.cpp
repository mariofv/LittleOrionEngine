#include "ExampleScript.h"
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
	

}

void ExampleScript::OnInspector(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Text("Testing Inspector");
	ImGui::Text("By Marco and Jordi guys, we are doing it, yeah!");
	
}

void ExampleScript::Test()
{
	//TODO
}
