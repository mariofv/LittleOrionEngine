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
#else
	CleanFolder();
	hot_reloading = true;
	std::thread(&DLLManager::CheckCompilation, this).detach();
	dll_file = App->filesystem->GetPath(std::string("/") + RESOURCES_SCRIPT_DLL_PATH);
	cr_plugin_open(hot_reloading_context, RESOURCES_SCRIPT_DLL_PATH);
	cr_plugin_update(hot_reloading_context);
	InitDLL();

	init_timestamp_dll = dll_file->GetModificationTimestamp();

	InitFolderTimestamps();
#endif	

}
#if !GAME
void DLLManager::InitFolderTimestamps()
{
	scripts_folder = App->filesystem->GetPath(std::string("/") + SCRIPT_PATH);
	init_timestamp_script_folder = scripts_folder->GetModificationTimestamp();
	scripts_timestamp_map.clear();
	for (const auto& script : scripts_folder->children)
	{
		uint32_t timestamp = script->GetModificationTimestamp();
		std::pair<Path*, uint32_t> script_timestamp (script, timestamp);
		scripts_timestamp_map.insert(script_timestamp);
	}
}

bool DLLManager::CheckFolderTimestamps()
{
	for (const auto& script : scripts_folder->children)
	{
		std::unordered_map<Path*, uint32_t>::const_iterator search = scripts_timestamp_map.find(script);
		if (search == scripts_timestamp_map.end())
		{
			uint32_t timestamp = script->GetModificationTimestamp();
			std::pair<Path*, uint32_t> script_timestamp(script, timestamp);
			scripts_timestamp_map.insert(script_timestamp);
			return true;
		}
		else 
		{
			uint32_t new_timestamp = search->first->GetModificationTimestamp();
			if (search->second != new_timestamp)
			{
				scripts_timestamp_map[search->first] = new_timestamp;
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
		hot_reloading = true;
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
	}
}

void DLLManager::CompileGameplayProject() const
{
	APP_LOG_INFO("Change detected in the Gameplay System, compilation in process.");
	std::wstring msbuild_path(MSBUILD_PATH);
	std::string msbuild_path_to_string(msbuild_path.begin(), msbuild_path.end());
	std::string command('\"' + msbuild_path_to_string + COMMAND_FOR_COMPILING);
	system(command.c_str());

}

void DLLManager::CheckCompilation()
{
	if (!compiling) 
	{
		compiling = true;
		std::thread compilator = std::thread(&DLLManager::CompileGameplayProject, this);
		compilator.join();
		last_timestamp_dll = dll_file->GetModificationTimestamp();
		if (App->filesystem->Exists(COMPILED_FOLDER_DLL_PATH))
		{
			if (App->filesystem->Exists(COMPILED_SCRIPT_DLL_PATH) && hot_reloading)
			{
				APP_LOG_INFO("Compilation was successful");
				hot_reloading = false;
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
		compiling = false;
	}
}

bool DLLManager::InitDLL()
{
	auto p = (cr_internal *)hot_reloading_context.p;
	gameplay_dll = (HMODULE)p->handle;
	if (gameplay_dll == nullptr) 
	{
		cr_plugin_update(hot_reloading_context);
		InitDLL();
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
#endif
void DLLManager::CleanUp()
{
#if GAME
	FreeLibrary(gameplay_dll);
	return;
#endif
	cr_plugin_close(hot_reloading_context);
	scripts_timestamp_map.clear();
	required_files.clear();
}

#if !GAME
void DLLManager::CleanFolder() const
{
	Path* resource_folder = App->filesystem->GetPath(std::string("/") + RESOURCES_SCRIPT_PATH);
	for (const auto& file : resource_folder->children)
	{
		bool found = std::find(required_files.begin(), required_files.end(), file->GetFilenameWithoutExtension()) != required_files.end();
		if(!found)
		{
			std::string filepath = file->GetFullPath();
			filepath.erase(filepath.begin()+0);
			remove(filepath.c_str());
		}
	}
}
#endif