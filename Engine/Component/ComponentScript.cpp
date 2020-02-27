#include "ComponentScript.h"
#include "Main/Application.h"
#include "Module/ModuleRender.h"
#include "Main/GameObject.h"
#include "Module/ModuleScriptManager.h"
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

void ComponentScript::LoadName(std::string script_name) 
{
	this->name = script_name;
	script = App->scripts->CreateResourceScript(script_name, owner);
	//App->scripts->scripts.push_back(this);
}

void ComponentScript::Update()
{
	if (script) 
	{
		script->Update();
	}
}

void ComponentScript::Delete()
{
	App->scripts->RemoveComponentScript(this);
}

void ComponentScript::ShowComponentWindow()
{
	if (script)
	{
		script->OnInspector(ImGui::GetCurrentContext());
	}
}

void ComponentScript::Save(Config & config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddInt((unsigned int)type, "ComponentType");
	config.AddBool(active, "Active");
	config.AddString(name, "ScriptName");

}

void ComponentScript::Load(const Config & config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);

	config.GetString("ScriptName", this->name, "");
	LoadName(this->name);


}