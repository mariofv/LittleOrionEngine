#include "ComponentScript.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleScriptManager.h"
#include "Module/ModuleTime.h"
#include "Script/Script.h"

#include <imgui.h>

ComponentScript::ComponentScript() : Component(nullptr, ComponentType::SCRIPT)
{

}
ComponentScript::ComponentScript(GameObject* owner, std::string& script_name) : Component(owner, ComponentType::SCRIPT)
{
	script = App->scripts->CreateResourceScript(script_name, owner);
	App->scripts->scripts.push_back(this);
	assert(script->owner);
}

ComponentScript & ComponentScript::operator=(const ComponentScript & component_to_copy)
{
	LoadName(component_to_copy.name);
	return *this;
}

Component* ComponentScript::Clone(GameObject* owner, bool original_prefab)
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
	CloneBase(static_cast<Component*>(created_component));
	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
	if(this->script) //We must have a script if copied has a script
	{
		created_component->script->owner = owner;
		assert(created_component->script->owner);
	}
	return created_component;
}

void ComponentScript::CopyTo(Component* component_to_copy) const
{
	*component_to_copy = *this;
	auto component_script = *static_cast<ComponentScript*>(component_to_copy);
	component_script = *this;
	component_script.script->owner = component_script.owner;
	assert(component_script.script->owner);
}

void ComponentScript::Enable()
{
	active = true;
	if(!awaken && App->time->isGameRunning())
	{
		AwakeScript();
		StartScript();
	}
}

void ComponentScript::LoadName(const std::string& script_name) 
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
		assert(script->owner);
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
		assert(script->owner);
	}
}