#define CR_HOST CR_SAFE
#include "ModuleScriptManager.h"

#include "Component/ComponentScript.h"
#include "Filesystem/Path.h"
#include "Filesystem/PathAtlas.h"
#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleTime.h"

#include "Script/Script.h"

#include <algorithm>

bool ModuleScriptManager::Init()
{
	APP_LOG_SECTION("************ Module Manager Script ************");
	GetCurrentPath();
#if GAME
	//TODO USE THE NEW FILESYSTEM TO DO THIS
	bool success = CopyFile(RESOURCES_SCRIPT_DLL_PATH, working_directory.c_str(), false);
	gameplay_dll = LoadLibrary(RESOURCE_SCRIPT_DLL_FILE);
	return true;
#endif

	dll_file = App->filesystem->GetPath(std::string("/") + RESOURCES_SCRIPT_DLL_PATH);
	scripts_list_file_path = App->filesystem->GetPath(RESOURCES_SCRIPT_PATH + std::string("/") + RESOURCES_SCRIPT_LIST_FILENAME);

	LoadScriptList();
	cr_plugin_open(ctx, RESOURCES_SCRIPT_DLL_PATH);
	cr_plugin_update(ctx);
	InitDLL();
	
	init_timestamp_dll = dll_file->GetModificationTimestamp();
	init_timestamp_script_list = scripts_list_file_path->GetModificationTimestamp();

	return true;
}

update_status ModuleScriptManager::Update()
{
	
	if (!scripts.empty() && App->time->isGameRunning()) 
	{
		RunScripts();
	}
	if (!App->time->isGameRunning()) 
	{
		last_timestamp_dll = dll_file->GetModificationTimestamp();
		if (last_timestamp_dll != init_timestamp_dll)
		{
			ReloadDLL();
			init_timestamp_dll = last_timestamp_dll;
		}

		last_timestamp_script_list = scripts_list_file_path->GetModificationTimestamp();
		if (last_timestamp_script_list != init_timestamp_script_list)
		{
			LoadScriptList();
			init_timestamp_script_list = last_timestamp_script_list;
		}
	}
	return update_status::UPDATE_CONTINUE;
}

bool ModuleScriptManager::CleanUp()
{
	
#if GAME
	FreeLibrary(gameplay_dll);
	return true;
#endif
	cr_plugin_close(ctx);

	return true;
}

void ModuleScriptManager::GetCurrentPath()
{
	//TODO:Move this
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	working_directory = NPath;
	working_directory += "/" + std::string(RESOURCE_SCRIPT_DLL_FILE);
}

void ModuleScriptManager::CreateScript(const std::string& name)
{
	FileData cpp_file_data = App->filesystem->GetPath(RESOURCES_SCRIPT_TEMPLATE_CPP)->GetFile()->Load();
	std::string cpp_file((char*)cpp_file_data.buffer, cpp_file_data.size);
	free((char*)cpp_file_data.buffer);

	FileData header_file_data = App->filesystem->GetPath(RESOURCES_SCRIPT_TEMPLATE_H)->GetFile()->Load();
	std::string header_file((char*)header_file_data.buffer, header_file_data.size);
	free((char*)header_file_data.buffer);

	Utils::ReplaceStringInPlace(cpp_file, "TemplateScript", name);
	Utils::ReplaceStringInPlace(header_file, "TemplateScript", name);

	std::string name_uppercase = name;
	std::transform(name_uppercase.begin(), name_uppercase.end(), name_uppercase.begin(), ::toupper);
	Utils::ReplaceStringInPlace(header_file, "_TEMPLATESCRIPT_H_", "_" + name_uppercase + "_H_");

	if (!App->filesystem->Exists((SCRIPT_PATH + name + ".cpp").c_str()))
	{
		//TODO: Use filesystem for this
		Utils::SaveFileContent(cpp_file, SCRIPT_PATH + name + ".cpp");
		Utils::SaveFileContent(header_file, SCRIPT_PATH + name + ".h");
		scripts_list.push_back(name);
		SaveScriptList();
	}
	
}

void ModuleScriptManager::InitResourceScript()
{
	if (gameplay_dll != nullptr)
	{
		for (const auto& component_script : scripts)
		{
			CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(gameplay_dll, (component_script->name + "DLL").c_str());
			if (script_func != nullptr)
			{
				delete component_script->script;
				component_script->script = script_func();
				component_script->script->AddReferences(component_script->owner, App);
				component_script->script->InitPublicGameObjects();
			}
		}
	}
}

Script* ModuleScriptManager::CreateResourceScript(const std::string& script_name, GameObject* owner) 
{
	if (gameplay_dll != nullptr)
	{
		CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(gameplay_dll, (script_name+"DLL").c_str());
		if (script_func != nullptr)
		{
			Script* script = script_func();
			script->AddReferences(owner, App);
			script->InitPublicGameObjects();
			return script;
		}
	}
	return nullptr;
}

ComponentScript* ModuleScriptManager::CreateComponentScript()
{
	ComponentScript* new_script = new ComponentScript();
	scripts.push_back(new_script);
	return new_script;
}

void ModuleScriptManager::RemoveComponentScript(ComponentScript* script_to_remove)
{
	const auto it = std::find(scripts.begin(), scripts.end(), script_to_remove);
	if (it != scripts.end())
	{
		delete *it;
		scripts.erase(it);
	}
}

void ModuleScriptManager::LoadScriptList() 
{
	if (scripts_list.size() > 0) 
	{
		scripts_list.clear();
	}
	
	FileData scripts_list_data = scripts_list_file_path->GetFile()->Load();
	char* scripts_file_data = (char*)scripts_list_data.buffer;
	size_t readed_bytes = scripts_list_data.size;
	if (scripts_file_data != nullptr)
	{
		std::string serialized_scripts_string = scripts_file_data;
		free(scripts_file_data);

		Config scripts_config(serialized_scripts_string);
		scripts_config.GetVector<std::string>("Scripts", scripts_list, std::vector<std::string>());
	}
}

void ModuleScriptManager::SaveScriptList()
{
	Config config;
	config.AddVector<std::string>(scripts_list, "Scripts");

	std::string serialized_script_list_string;
	config.GetSerializedString(serialized_script_list_string);

	Path* script_resources_path = App->filesystem->GetPath(RESOURCES_SCRIPT_PATH);
	script_resources_path->Save(RESOURCES_SCRIPT_LIST_FILENAME, serialized_script_list_string);
}

void ModuleScriptManager::InitScripts()
{
	for (const auto& component_script : scripts)
	{
		component_script->AwakeScript();
	}
	for (const auto& component_script : scripts)
	{
		component_script->StartScript();
	}
}

void ModuleScriptManager::RunScripts()
{
	for (const auto& component_script : scripts)
	{
		component_script->Update();
	}
}

void ModuleScriptManager::RemoveScriptPointers()
{
	for (const auto& component_script : scripts)
	{
		component_script->script = nullptr;
	}
}

void ModuleScriptManager::InitDLL()
{
	auto p = (cr_internal *)ctx.p;
	assert(p->handle);
	gameplay_dll = (HMODULE)p->handle;
}

void ModuleScriptManager::ReloadDLL() 
{
	std::unordered_map<uint64_t, Config> config_list;
	SaveVariables(config_list);
	if (gameplay_dll != nullptr) 
	{

		cr_plugin_update(ctx);


		RemoveScriptPointers();
		InitDLL();
	}
	InitResourceScript();
	LoadVariables(config_list);
}

void ModuleScriptManager::Refresh()
{
	LoadScriptList();
	ReloadDLL();
}

void ModuleScriptManager::ReLink()
{
	for (const auto& component_script : scripts)
	{
		component_script->script->Link();
	}
}

void ModuleScriptManager::SaveVariables(std::unordered_map<uint64_t, Config>& config_list)
{
	for (const auto& component_script : scripts)
	{
		if (component_script->script != nullptr) 
		{
			Config config;
			component_script->script->Save(config);
			config_list.insert({ component_script->UUID, config });
		}

	}
}

void ModuleScriptManager::LoadVariables(std::unordered_map<uint64_t, Config> config_list)
{
	for (const auto& component_script : scripts)
	{
		if (component_script->script != nullptr)
		{
			std::unordered_map<uint64_t, Config>::const_iterator got = config_list.find(component_script->UUID);
			if (got != config_list.end()) {
				component_script->script->Load(got->second);
				component_script->script->Link();
			}
		}
		
	}
}