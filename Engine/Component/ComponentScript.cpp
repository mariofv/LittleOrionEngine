#include "ComponentScript.h"
#include "Main/Application.h"
#include "Module/ModuleRender.h"
#include "Main/GameObject.h"
#include "Module/ModuleScriptManager.h"
//#include "../GamePlaySystem/src/Script/Script.h"
#include "Script/Script.h"
#include "imgui.h"

ComponentScript::ComponentScript() : Component(nullptr, ComponentType::SCRIPT)
{

}
ComponentScript::ComponentScript(GameObject * owner, std::string script_name) : Component(owner, ComponentType::SCRIPT)
{
	script = App->scripts->CreateResourceScript(script_name, owner);
	App->scripts->scripts.push_back(this);
}

ComponentScript::~ComponentScript()
{
}
void ComponentScript::LoadName(std::string script_name) {
	this->name = script_name;
	script = App->scripts->CreateResourceScript(script_name, owner);
	App->scripts->scripts.push_back(this);
}
void ComponentScript::Update()
{
	script->Update();
}
void ComponentScript::Delete()
{
	App->scripts->RemoveComponentScript(this);
}

void ComponentScript::ShowComponentWindow()
{
	script->OnInspector(ImGui::GetCurrentContext());
}
