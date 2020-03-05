#include "ModuleScriptManager.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleFileSystem.h"
#include "Component/ComponentScript.h"
#include "Script/Script.h"
#include "Filesystem/File.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>


bool ModuleScriptManager::Init()
{
	APP_LOG_SECTION("************ Module Manager Script ************");
	//TODO: Load all the .dll
	GetCurrentPath();
	//MoveFile(DLL_PATH, working_directory.c_str());
	App->filesystem->Copy(DLL_PATH, working_directory.c_str());
	dll_file = std::make_unique<File>("Resources/Scripts/GamePlaySystem.dll");
	init_timestamp = dll_file->modification_timestamp;
	gameplay_dll = LoadLibrary("GamePlaySystem_test.dll");
	LoadScriptList();

	return true;
}

update_status ModuleScriptManager::Update()
{
	
	if (!scripts.empty()) 
	{
		for (auto &component_script : scripts) 
		{
			component_script->Update();
		}
	}

	PHYSFS_Stat file_info;
	PHYSFS_stat(dll_file->file_path.c_str(), &file_info);
	last_timestamp = file_info.modtime;
	//File dll_test("Resources/Scripts/GamePlaySystem.dll");
	if (last_timestamp != init_timestamp) 
	{
		CreatePDB();
		ReloadDLL();
		init_timestamp = last_timestamp;
	}
	
	return update_status::UPDATE_CONTINUE;
}

bool ModuleScriptManager::CleanUp()
{
	FreeLibrary(gameplay_dll);
	return true;
}

void ModuleScriptManager::InitResourceScript() 
{
	if (gameplay_dll != nullptr)
	{
		for (auto &component_script : scripts)
		{
			CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(gameplay_dll, (component_script->name + "DLL").c_str());
			if (script_func != nullptr)
			{
				component_script->script = script_func();
				component_script->script->AddReferences(component_script->owner, App);
			}
		}
	}
}

Script* ModuleScriptManager::CreateResourceScript( const std::string& script_name, GameObject* owner) 
{
	if (gameplay_dll != nullptr)
	{
		CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(gameplay_dll, (script_name+"DLL").c_str());
		if (script_func != nullptr)
		{
			Script* script = script_func();
			script->AddReferences(owner, App);
			return script;
		}
		return nullptr;
	}
	return nullptr;
}

ComponentScript* ModuleScriptManager::CreateComponentScript()
{
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

void ModuleScriptManager::LoadScriptList() 
{
	if(scripts_list.size()>0)
		scripts_list.clear();

	size_t readed_bytes;
	char* scripts_file_data = App->filesystem->Load(SCRIPT_LIST_PATH, readed_bytes);
	if (scripts_file_data != nullptr)
	{
		std::string serialized_scripts_string = scripts_file_data;
		free(scripts_file_data);

		Config scripts_config(serialized_scripts_string);

		std::vector<Config> scripts_list_configs;
		scripts_config.GetChildrenConfig("Scripts", scripts_list_configs);
		for (unsigned int i = 0; i < scripts_list_configs.size(); ++i)
		{
			std::string script;
			scripts_list_configs[i].GetString("Script", script,"");
			scripts_list.push_back(script);
		}
	}

}

void ModuleScriptManager::ReloadDLL() 
{
	if (gameplay_dll != nullptr) 
	{
		if (!FreeLibrary(gameplay_dll)) 
		{
			return;
		}
		else 
		{
			for (auto &component_script : scripts)
			{
				component_script->script = nullptr;
			}
			remove("GamePlaySystem_test.dll");
		}
	}
	//MoveFile(DLL_PATH, working_directory.c_str());
	App->filesystem->Copy(DLL_PATH, working_directory.c_str());
	gameplay_dll = LoadLibrary("GamePlaySystem_test.dll");
	InitResourceScript();

}

void ModuleScriptManager::CreatePDB() 
{
	App->filesystem->Copy("../GamePlaySystem/Debug/GamePlaySystem.pdb", "../GamePlaySystem/Debug/GamePlaySystem_test.pdb");
}

void ModuleScriptManager::GetCurrentPath() 
{
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	working_directory = NPath;
	working_directory += "/GamePlaySystem_test.dll";
}

