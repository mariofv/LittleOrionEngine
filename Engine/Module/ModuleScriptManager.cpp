#include "ModuleScriptManager.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Component/ComponentScript.h"
#include "Script/Script.h"

bool ModuleScriptManager::Init()
{
	APP_LOG_SECTION("************ Module Manager Script ************");
	//TODO: Load all the .dll
	gp_dll = LoadLibrary("GamePlaySystem.dll");
	//TODO: fill / load the component script vector.
	InitResourceScript();
	return true;
}

update_status ModuleScriptManager::Update()
{
	

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScriptManager::CleanUp()
{
	FreeLibrary(gp_dll);
	return true;
}
void ModuleScriptManager::InitResourceScript() {
	if (gp_dll != nullptr)
	{
		for (auto &component_script : scripts)
		{
			CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(gp_dll, (component_script->name + "DLL").c_str());
			if (script_func != nullptr)
			{
				component_script->script = script_func();
				//component_script->script->AddReferences(component_script->owner, App);
			}
		}
	}
}
Script* ModuleScriptManager::CreateResourceScript( std::string script_name, GameObject* owner) {
	if (gp_dll != nullptr)
	{
		CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(gp_dll, (script_name+"DLL").c_str());
		if (script_func != nullptr)
		{
			Script* script = script_func();
			//script->AddReferences(owner, App);
			return script;
		}
		return nullptr;
	}
	return nullptr;
}
ComponentScript* ModuleScriptManager::CreateComponentScript() {
	ComponentScript* new_script = new ComponentScript();
	scripts.push_back(new_script);
	return new_script;
}

void ModuleScriptManager::RemoveComponentScript(ComponentScript * script_to_remove)
{
	auto it = std::find(scripts.begin(), scripts.end(), script_to_remove);
	if (it != scripts.end())
	{
		delete *it;
		scripts.erase(it);
	}
}


