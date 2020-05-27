#include "ComponentScript.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScriptManager.h"
#include "Script/Script.h"

#include "imgui.h"

ComponentScript::ComponentScript() : Component(nullptr, ComponentType::SCRIPT)
{

}
ComponentScript::ComponentScript(GameObject* owner, std::string& script_name) : Component(owner, ComponentType::SCRIPT)
{
	script = App->scripts->CreateResourceScript(script_name, owner);
	App->scripts->scripts.push_back(this);
}

Component* ComponentScript::Clone(bool original_prefab) const
{
	ComponentScript * created_component;
	if (original_prefab)
	{
		created_component = new ComponentScript();
	}
	else
	{
		created_component = App->scripts->CreateComponentScript();
	}
	*created_component = *this;
	return created_component;
};

void ComponentScript::Copy(Component * component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentScript*>(component_to_copy) = *this;
}

void ComponentScript::Enable()
{
	active = true;
	if(!awaken)
	{
		AwakeScript();
		StartScript();
	}
}

void ComponentScript::LoadName(std::string& script_name) 
{
	this->name = script_name;
	script = App->scripts->CreateResourceScript(script_name, owner);
}

void ComponentScript::Update()
{
	if (script && active && awaken && started)
	{
		script->Update();
	}
}

void ComponentScript::AwakeScript()
{
	if (script && active)
	{
		script->Awake();
		awaken = true;
	}
}

void ComponentScript::StartScript()
{
	if (script && awaken && active)
	{
		started = true;
		script->Start();
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

void ComponentScript::SpecializedSave(Config& config) const
{
	config.AddString(name, "ScriptName");
	if (script)
	{
		script->Save(config);
	}
	
}

void ComponentScript::SpecializedLoad(const Config& config)
{
	config.GetString("ScriptName", this->name, "");
	LoadName(this->name);
	if (script)
	{
		script->Load(config);
	}
}