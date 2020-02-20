#include "ModuleManagerScript.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Component/ComponentScript.h"
#include "Script/Script.h"

bool ModuleManagerScript::Init()
{
	APP_LOG_SECTION("************ Module Manager Script ************");
	//TODO: Load all the .dll
	gp_dll = LoadLibrary("GamePlaySystem.dll");
	//TODO: fill the vector.
	return true;
}

// Called every draw update
update_status ModuleManagerScript::PreUpdate()
{
	//TODO Load libray only if file change
	if (gp_dll)
	{
		for (auto &script : scripts)
		{
			CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(gp_dll, script->name.c_str());
			if (script_func)
			{
				script->script = script_func();
			}
		}
	}
	return update_status::UPDATE_CONTINUE;
}
update_status ModuleManagerScript::Update()
{
	
	
	return update_status::UPDATE_CONTINUE;
}

bool ModuleManagerScript::CleanUp()
{
	FreeLibrary(gp_dll);
	return true;
}

ComponentScript* ModuleManagerScript::CreateComponentScript() {
	ComponentScript* new_script = new ComponentScript();
	scripts.push_back(new_script);
	return new_script;
}


