#define CR_HOST CR_SAFE //must be here for HOT RELOADING library.
#include "DLLManager.h"

#include "Filesystem/Path.h"
#include "Filesystem/PathAtlas.h"
#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

#include <thread>

DLLManager::DLLManager()
{

#if GAME
	//TODO USE THE NEW FILESYSTEM TO DO THIS
	Utils::GetCurrentPath(working_directory);
	working_directory += "/" + std::string(RESOURCE_SCRIPT_DLL_FILE);
	CopyFile(RESOURCES_SCRIPT_DLL_PATH, working_directory.c_str(), false);
	gameplay_dll = LoadLibrary(RESOURCE_SCRIPT_DLL_FILE);
	return true;
#endif

	dll_file = App->filesystem->GetPath(std::string("/") + RESOURCES_SCRIPT_DLL_PATH);//RENAME DEFINED NAMES
	cr_plugin_open(hot_reloading_context, RESOURCES_SCRIPT_DLL_PATH);
	cr_plugin_update(hot_reloading_context);
	InitDLL();

	init_timestamp_dll = dll_file->GetModificationTimestamp();

	InitFolderTimestamps();
	

}

void DLLManager::InitFolderTimestamps()
{
	scripts_folder = App->filesystem->GetPath("/Assets/Scripts/src/Script");
	init_timestamp_script_folder = scripts_folder->GetModificationTimestamp();

	for (const auto& script : scripts_folder->children)
	{
		uint32_t timestamp = script->GetModificationTimestamp();
		std::pair<Path*, uint32_t> script_timestamp (script, timestamp);
		scripts_timestamp_map.insert(script_timestamp);
	}
}

bool DLLManager::CheckFolderTimestamps()
{
	//scripts_folder = App->filesystem->GetPath("/Assets/Scripts/src/Script");
	//init_timestamp_script_folder = scripts_folder->GetModificationTimestamp();
	for (const auto& script : scripts_folder->children)
	{
		std::unordered_map<Path*, uint32_t>::const_iterator search = scripts_timestamp_map.find(script);
		if (search == scripts_timestamp_map.end())
		{
			uint32_t timestamp = script->GetModificationTimestamp();
			std::pair<Path*, uint32_t> script_timestamp(script, timestamp);
			scripts_timestamp_map.insert(script_timestamp);
			APP_LOG_INFO("HEY NEW FILE FOUND");
			return true;
		}
		else 
		{
			uint32_t new_timestamp = search->first->GetModificationTimestamp();
			if (search->second != new_timestamp)
			{
				scripts_timestamp_map[search->first] = new_timestamp;
				APP_LOG_INFO("HEY NEW FILE MODIFIED");
				return true;
			}
		}

	}
	return false;
}

bool DLLManager::DLLItsUpdated()
{

	last_timestamp_dll = dll_file->GetModificationTimestamp();
	if (last_timestamp_dll != init_timestamp_dll)
	{
		init_timestamp_dll = last_timestamp_dll;
		InitFolderTimestamps();
		return true;
	}

	return false;
}

void DLLManager::CheckGameplayFolderStatus()
{
	last_timestamp_script_folder = scripts_folder->GetModificationTimestamp();
	if (last_timestamp_script_folder != init_timestamp_script_folder || CheckFolderTimestamps())
	{
		std::thread(&DLLManager::CheckCompilation, this).detach();
		InitFolderTimestamps();
		//init_timestamp_script_folder = last_timestamp_script_folder;
	}


}

void DLLManager::CompileGameplayProject()
{
	APP_LOG_INFO("NOW I'M GOING TO COMPILE! TRUST ME!");
	std::wstring ws(MSBUILD_PATH);
	std::string test(ws.begin(), ws.end());
	std::string aux('\"' + test + "\\MSBuild.exe\" Assets\\Scripts\\GameplaySystem.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86");
	system(aux.c_str());

}

void DLLManager::CheckCompilation()
{
	std::thread compilator = std::thread(&DLLManager::CompileGameplayProject, this);
	compilator.join();
	if (App->filesystem->Exists("/Assets/Scripts/Debug")) 
	{
		if (App->filesystem->Exists("/Assets/Scripts/Debug/GameplaySystemDebug.dll"))
		{
			APP_LOG_SUCCESS("Compiled Scripts Correctly!");
		}
		else
		{
			APP_LOG_ERROR("Error compiling scripts, do it manually to check errors");
		}
	}
	else
	{
		APP_LOG_ERROR("Command error, not compiled");
	}
}

bool DLLManager::InitDLL()
{
	auto p = (cr_internal *)hot_reloading_context.p;
	assert(p->handle);
	gameplay_dll = (HMODULE)p->handle;
	if (gameplay_dll == nullptr) 
	{
		return false;
	}
	return true;
}

bool DLLManager::ReloadDLL()
{
	if (gameplay_dll != nullptr)
	{

		cr_plugin_update(hot_reloading_context);
		if(InitDLL())
		{
			return true;
		}
	}
	return false;
}

void DLLManager::CleanUp()
{
#if GAME
	FreeLibrary(gameplay_dll);
	return true;
#endif
	cr_plugin_close(hot_reloading_context);
}
