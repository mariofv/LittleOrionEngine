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
	CopyFile(SCRIPTS_DLL_PATH, working_directory.c_str(), false);
	gameplay_dll = LoadLibrary(SCRIPT_DLL_FILE);
	return true;
#endif

	dll_file = App->filesystem->GetPath(std::string("/") + RESOURCES_SCRIPT_DLL_PATH);
	scripts_list_file_path = App->filesystem->GetPath(RESOURCES_SCRIPT_PATH + std::string("/") + RESOURCES_SCRIPT_LIST_FILENAME);

	LoadScriptList();
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
		//TODO Check it not every frame.
		last_timestamp_dll = TimeStamp(dll_file->GetFullPath().c_str());
		if (last_timestamp_dll != init_timestamp_dll)
		{
			ReloadDLL();
			init_timestamp_dll = last_timestamp_dll;
		}

		last_timestamp_script_list = TimeStamp(scripts_list_file_path->GetFullPath().c_str());
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
	FreeLibrary(gameplay_dll);
	return true;
}

long ModuleScriptManager::TimeStamp(const char* path)
{
	PHYSFS_Stat file_info;
	PHYSFS_stat(path, &file_info);
	return file_info.modtime;
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
	PatchDLL(RESOURCES_SCRIPT_DLL_PATH, working_directory.c_str());
	gameplay_dll = LoadLibrary(RESOURCE_SCRIPT_DLL_FILE);
}

void ModuleScriptManager::ReloadDLL() 
{
	std::unordered_map<uint64_t, Config> config_list;
	SaveVariables(config_list);
	if (gameplay_dll != nullptr) 
	{

		if (!FreeLibrary(gameplay_dll)) 
		{
			return;
		}
		else 
		{
			RemoveScriptPointers();
			remove(RESOURCE_SCRIPT_DLL_FILE);
			InitDLL();
		}
		
	}
	InitResourceScript();
	LoadVariables(config_list);
}

bool ModuleScriptManager::PatchDLL(const char* dll_path, const char* patched_dll_path)
{
	// init
	char patched_pdb_path[MAX_PATH] = { '\0' };

	// open DLL and copy content to file_content for easy parsing of the DLL content
	DWORD byte_read;
	HANDLE file = CreateFileA(dll_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) 
	{
		APP_LOG_ERROR("Failed to create file.\n");
		return false;
	}
		
	size_t file_size = GetFileSize((HANDLE)file, NULL);
	BYTE* file_content = (BYTE*)malloc(file_size);
	bool is_file_read_ok = ReadFile((HANDLE)file, file_content, (DWORD)file_size, &byte_read, NULL);
	CloseHandle(file);
	if (!is_file_read_ok || byte_read != file_size) 
	{
		APP_LOG_ERROR("Failed to read file.\n");
	}

	// check signature
	IMAGE_DOS_HEADER dos_header = *(IMAGE_DOS_HEADER*)file_content;
	if (dos_header.e_magic != IMAGE_DOS_SIGNATURE)
	{
		APP_LOG_ERROR("Not IMAGE_DOS_SIGNATURE\n");
	}
	// IMAGE_NT_HEADERS
	IMAGE_NT_HEADERS nt_header = *((IMAGE_NT_HEADERS*)(file_content + dos_header.e_lfanew));
	if (nt_header.Signature != IMAGE_NT_SIGNATURE) 
	{
		APP_LOG_ERROR("Not IMAGE_NT_SIGNATURE\n");
	}
	
	IMAGE_DATA_DIRECTORY debug_dir;
	if (nt_header.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC && nt_header.FileHeader.SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER)) 
	{
		debug_dir = nt_header.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
	}
	else 
	{
		APP_LOG_ERROR("Not IMAGE_NT_OPTIONAL_HDR_MAGIC\n");
	}
		

	if (debug_dir.VirtualAddress == 0 || debug_dir.Size == 0) 
	{
		APP_LOG_ERROR("No IMAGE_DIRECTORY_ENTRY_DEBUG data\n");
	}
		
	// find debug section
	int	debug_dir_section_index = -1;
	IMAGE_SECTION_HEADER* all_section_headers = (IMAGE_SECTION_HEADER*)(file_content + dos_header.e_lfanew + sizeof(IMAGE_NT_HEADERS));
	for (int j = 0; j < nt_header.FileHeader.NumberOfSections; ++j)
	{
		IMAGE_SECTION_HEADER section_header = all_section_headers[j];
		if ((debug_dir.VirtualAddress >= section_header.VirtualAddress) && (debug_dir.VirtualAddress < section_header.VirtualAddress + section_header.Misc.VirtualSize))
		{
			debug_dir_section_index = j;
			break;
		}
	}

	// read debug section
	char* pdb_path = nullptr;
	char original_pdb_path[MAX_PATH];
	if (debug_dir_section_index != -1)
	{
		// loop all debug directory
		int	num_debug_dir = debug_dir.Size / (int)sizeof(IMAGE_DEBUG_DIRECTORY);
		IMAGE_SECTION_HEADER section_header = all_section_headers[debug_dir_section_index];
		IMAGE_DEBUG_DIRECTORY* all_image_debug_dir = (IMAGE_DEBUG_DIRECTORY*)(file_content + debug_dir.VirtualAddress - (section_header.VirtualAddress - section_header.PointerToRawData));
		for (int i = 0; i < num_debug_dir; ++i)
		{
			IMAGE_DEBUG_DIRECTORY image_debug_dir = all_image_debug_dir[i];
			if (image_debug_dir.Type == IMAGE_DEBUG_TYPE_CODEVIEW)
			{
				DWORD signature = *((DWORD*)(file_content + image_debug_dir.PointerToRawData));
				if (signature == 'SDSR')	// RSDS type, i.e. PDB70
				{
					CV_INFO_PDB70* debug_info = ((CV_INFO_PDB70*)(file_content + image_debug_dir.PointerToRawData));
					pdb_path = (char*)debug_info->PdbFileName;
					strcpy(original_pdb_path, pdb_path);
					break;
				}
			}
		}
	}

	if (pdb_path == nullptr) 
	{
		APP_LOG_ERROR("No debug section is found.\n");
	}
		
	// Create new DLL and pdb
	Utils::PatchFileName(pdb_path);

	strcpy(patched_pdb_path, pdb_path);
	//App->filesystem->Copy(original_pdb_path, pdb_path);		// Copy new PDB
	CopyFile(original_pdb_path, pdb_path,true);

	HANDLE patched_dll = CreateFile(patched_dll_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD byte_write;
	WriteFile(patched_dll, file_content, (DWORD)file_size, &byte_write, nullptr);	// Generate patched DLL which points to the new PDB
	CloseHandle(patched_dll);

	// clean up
	APP_LOG_INFO("Patching DLL Succeeded.\n");
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