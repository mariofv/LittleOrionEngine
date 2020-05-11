#include "ModuleScriptManager.h"

#include "Component/ComponentScript.h"
#include "DLL/DLLManager.h"
#include "Filesystem/Path.h"
#include "Filesystem/PathAtlas.h"
#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleFileSystem.h"
#include "Module/ModuleTime.h"

#include "Script/Script.h"

#include <algorithm>
#include <Brofiler/Brofiler.h>
#include <fstream>

bool ModuleScriptManager::Init()
{
	APP_LOG_SECTION("************ Module Manager Script ************");

	dll = std::make_shared<DLLManager>();
#if !GAME
	LoadScriptList();
#endif
	return true;
}

update_status ModuleScriptManager::Update()
{
	
	if (!scripts.empty() && App->time->isGameRunning()) 
	{
		RunScripts();
	}
#if !GAME
	if (!App->time->isGameRunning()) 
	{
		if(dll->DLLItsUpdated())
		{
			hot_reloading = true;
			ReloadDLL();
		}
	}
#endif
	return update_status::UPDATE_CONTINUE;
}

bool ModuleScriptManager::CleanUp()
{
	
	dll->CleanUp();

	return true;
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

void ModuleScriptManager::CreateScript(const std::string& name)
{
	FileData cpp_file_data = App->filesystem->GetPath(RESOURCES_SCRIPT_TEMPLATE_CPP)->GetFile()->Load();
	std::string cpp_file((char*)cpp_file_data.buffer, cpp_file_data.size);
	delete[] cpp_file_data.buffer;

	FileData header_file_data = App->filesystem->GetPath(RESOURCES_SCRIPT_TEMPLATE_H)->GetFile()->Load();
	std::string header_file((char*)header_file_data.buffer, header_file_data.size);
	delete[] header_file_data.buffer;

	Utils::ReplaceStringInPlace(cpp_file, "TemplateScript", name);
	Utils::ReplaceStringInPlace(header_file, "TemplateScript", name);

	std::string name_uppercase = name;
	std::transform(name_uppercase.begin(), name_uppercase.end(), name_uppercase.begin(), ::toupper);
	Utils::ReplaceStringInPlace(header_file, "_TEMPLATESCRIPT_H_", "_" + name_uppercase + "_H_");

	if (!App->filesystem->Exists((std::string("/") + SCRIPT_PATH + std::string("/") + name + ".cpp").c_str()))
	{
		//TODO: Use filesystem for this
		Utils::SaveFileContent(cpp_file, SCRIPT_PATH + std::string("/") + name + ".cpp");
		Utils::SaveFileContent(header_file, SCRIPT_PATH + std::string("/") + name + ".h");
		scripts_list.push_back(name);
		UpdateGameplayProject(name);
	}

}

void ModuleScriptManager::InitResourceScript()
{
	if (dll->gameplay_dll != nullptr)
	{
		for (const auto& component_script : scripts)
		{
			CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(dll->gameplay_dll, (component_script->name + "DLL").c_str());
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
	if (dll->gameplay_dll != nullptr)
	{
		CREATE_SCRIPT script_func = (CREATE_SCRIPT)GetProcAddress(dll->gameplay_dll, (script_name+"DLL").c_str());
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

void ModuleScriptManager::LoadScriptList() 
{
	if (scripts_list.size() > 0) 
	{
		scripts_list.clear();
	}
	Path* scripts_list_file_path = App->filesystem->GetPath("/Assets/Scripts/src/Script");

	for(const auto& script : scripts_list_file_path->children )
	{
		std::string script_to_search = script->GetFilenameWithoutExtension();
		bool found = std::find(scripts_list.begin(), scripts_list.end(), script_to_search) != scripts_list.end();
		if(!found)
		{
			scripts_list.emplace_back(script_to_search);
		}
	}

}

void ModuleScriptManager::InitScripts()
{
	for (size_t i = 0; i < scripts.size(); ++i)
	{
		scripts[i]->AwakeScript();
	}
	for (size_t i = 0; i < scripts.size(); ++i)
	{
		scripts[i]->StartScript();
	}
}

void ModuleScriptManager::RunScripts()
{
	BROFILER_CATEGORY("Run Scripts", Profiler::Color::Aqua);
	for (const auto& component_script : scripts)
	{
		BROFILER_CATEGORY("Script", Profiler::Color::Lavender);
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

void ModuleScriptManager::ReloadDLL() 
{
#if !GAME
	std::unordered_map<uint64_t, Config> config_list;
	SaveVariables(config_list);
	if (dll->gameplay_dll != nullptr) 
	{
		dll->ReloadDLL();
		RemoveScriptPointers();
		dll->InitDLL();
	}
	dll->InitFolderTimestamps();
	InitResourceScript();
	LoadVariables(config_list);
	hot_reloading = false;
#endif
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

void ModuleScriptManager::CheckGameplayFolderStatus()
{
#if !GAME
	if(!hot_reloading)
	{
		dll->CheckGameplayFolderStatus();
	}
#endif
}

void ModuleScriptManager::UpdateGameplayProject(const std::string& script_name)
{
	std::ifstream gameplay_project("Assets/Scripts/GameplaySystem.vcxproj");
	std::string lines;
	std::vector<std::string> text;
	while (std::getline(gameplay_project, lines))
	{
		if (lines.size() > 0)
		{
			text.push_back(lines);
		}
	}
	bool compile = false;
	bool include = false;
	int count = 0;
	int include_position = 0;
	int compile_position = 0;
	for (auto it = begin(text); it != end(text); ++it)
	{
		++count;
		if (!include && it->find("<ClInclude Include=") != std::string::npos)
		{
			include_position = count + 1;
			include = true;
		}
		if (!compile && it->find("<ClCompile Include=") != std::string::npos)
		{
			compile_position = count + 1;
			compile = true;
		}
		if (compile && include)
		{
			break;
		}
	}
	text.insert(text.begin() + (include_position), { "    <ClInclude Include=\"src\\Script\\" + script_name + ".h\" />" });
	text.insert(text.begin() + (compile_position), { "    <ClCompile Include=\"src\\Script\\" + script_name + ".cpp\" />" });
	std::ofstream out_file("Assets/Scripts/GameplaySystem.vcxproj");
	for (const auto &line : text) out_file << line << "\n";
}