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
	//App->filesystem->Copy(DLL_PATH, working_directory.c_str());
	char patchedPdbPath[MAX_PATH] = { '\0' };
	patchDLL(DLL_PATH, working_directory.c_str(), patchedPdbPath);
	dll_file = std::make_unique<File>("Resources/Scripts/GamePlaySystem.dll");
	init_timestamp = dll_file->modification_timestamp;
	gameplay_dll = LoadLibrary("GamePlaySyste_.dll");
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
			remove("GamePlaySyste_.dll");
		}
	}
	//CreatePDB();
	//MoveFile(DLL_PATH, working_directory.c_str());
	//App->filesystem->Copy(DLL_PATH, working_directory.c_str());
	char patchedPdbPath[MAX_PATH] = { '\0' };
	patchDLL(DLL_PATH, working_directory.c_str(), patchedPdbPath);
	gameplay_dll = LoadLibrary("GamePlaySyste_.dll");
	InitResourceScript();

}

void ModuleScriptManager::CreatePDB() 
{
	App->filesystem->Copy("../GamePlaySystem/Debug/GamePlaySystem.pdb", "../GamePlaySystem/Debug/GamePlaySyste_.pdb");
}

void ModuleScriptManager::GetCurrentPath() 
{
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	working_directory = NPath;
	working_directory += "/GamePlaySyste_.dll";
	APP_LOG_ERROR("something here");
}
size_t ModuleScriptManager::CStrlastIndexOfChar(const char* str, char findChar)
{
	intptr_t i = strlen(str) - 1;
	while (i >= 0)
	{
		if (str[i] == findChar)
			return i;
		--i;
	}
	return (size_t)-1;
}
bool ModuleScriptManager::patchFileName(char* fileName)
{
	size_t	dotIdx = CStrlastIndexOfChar(fileName, '.');
	if (dotIdx != (size_t)-1)
	{
		fileName[dotIdx - 1] = '_';
		return true;
	}
	else
		return false;
}


bool ModuleScriptManager::FileIOisExist(const char* fileName)
{
	DWORD dwAttrib = GetFileAttributes(fileName);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool ModuleScriptManager::FileIOcopy(const char* fromFile, const char* toFile, bool overwriteExisting)
{
	return CopyFile(fromFile, toFile, !overwriteExisting);
}

bool ModuleScriptManager::patchDLL(const char* dllPath, const char patchedDllPath[MAX_PATH], char patchedPdbPath[MAX_PATH])
{
	// init
	patchedPdbPath[0] = '\0';

	// open DLL and copy content to fileContent for easy parsing of the DLL content
	DWORD byteRead;
	HANDLE file = CreateFileA(dllPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return false;
	size_t	fileSize = GetFileSize((HANDLE)file, NULL);
	BYTE*	fileContent = (BYTE*)malloc(fileSize);
	bool	isFileReadOk = ReadFile((HANDLE)file, fileContent, (DWORD)fileSize, &byteRead, NULL);
	CloseHandle(file);
	if (!isFileReadOk || byteRead != fileSize)
		APP_LOG_ERROR("Failed to read file.\n", false);

	// check signature
	IMAGE_DOS_HEADER dosHeader = *(IMAGE_DOS_HEADER*)fileContent;
	if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
		APP_LOG_ERROR("Not IMAGE_DOS_SIGNATURE\n", false);

	// IMAGE_NT_HEADERS
	IMAGE_NT_HEADERS ntHeader = *((IMAGE_NT_HEADERS*)(fileContent + dosHeader.e_lfanew));
	if (ntHeader.Signature != IMAGE_NT_SIGNATURE)
		APP_LOG_ERROR("Not IMAGE_NT_SIGNATURE\n", false);

	IMAGE_DATA_DIRECTORY debugDir;
	if (ntHeader.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR_MAGIC && ntHeader.FileHeader.SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER))
		debugDir = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
	else
		APP_LOG_ERROR("Not IMAGE_NT_OPTIONAL_HDR_MAGIC\n", false);

	if (debugDir.VirtualAddress == 0 || debugDir.Size == 0)
		APP_LOG_ERROR("No IMAGE_DIRECTORY_ENTRY_DEBUG data\n", false);

	// find debug section
	int						debugDirSectionIdx = -1;
	IMAGE_SECTION_HEADER*	allSectionHeaders = (IMAGE_SECTION_HEADER*)(fileContent + dosHeader.e_lfanew + sizeof(IMAGE_NT_HEADERS));
	for (int j = 0; j < ntHeader.FileHeader.NumberOfSections; ++j)
	{
		IMAGE_SECTION_HEADER sectionHeader = allSectionHeaders[j];
		if ((debugDir.VirtualAddress >= sectionHeader.VirtualAddress) &&
			(debugDir.VirtualAddress < sectionHeader.VirtualAddress + sectionHeader.Misc.VirtualSize))
		{
			debugDirSectionIdx = j;
			break;
		}
	}

	// read debug section
	char*	pdbPath = nullptr;
	char	originalPdbPath[MAX_PATH];
	if (debugDirSectionIdx != -1)
	{
		// loop all debug directory
		int						numDebugDir = debugDir.Size / (int)sizeof(IMAGE_DEBUG_DIRECTORY);
		IMAGE_SECTION_HEADER	sectionHeader = allSectionHeaders[debugDirSectionIdx];
		IMAGE_DEBUG_DIRECTORY*	allImageDebugDir = (IMAGE_DEBUG_DIRECTORY*)(fileContent + debugDir.VirtualAddress - (sectionHeader.VirtualAddress - sectionHeader.PointerToRawData));
		for (int i = 0; i < numDebugDir; ++i)
		{
			IMAGE_DEBUG_DIRECTORY imageDebugDir = allImageDebugDir[i];
			if (imageDebugDir.Type == IMAGE_DEBUG_TYPE_CODEVIEW)
			{
				DWORD signature = *((DWORD*)(fileContent + imageDebugDir.PointerToRawData));
				if (signature == 'SDSR')	// RSDS type, i.e. PDB70
				{
					CV_INFO_PDB70* debugInfo = ((CV_INFO_PDB70*)(fileContent + imageDebugDir.PointerToRawData));
					pdbPath = (char*)debugInfo->PdbFileName;
					strcpy(originalPdbPath, pdbPath);
					break;
				}
			}
		}
	}

	if (pdbPath == nullptr)
		APP_LOG_ERROR("No debug section is found.\n", false);

	// create new DLL and pdb
	patchFileName(pdbPath);

	if (FileIOisExist(originalPdbPath))
	{
		strcpy(patchedPdbPath, pdbPath);
		FileIOcopy(originalPdbPath, pdbPath, true);		// copy new PDB
	}
	HANDLE patchedDLL = CreateFile(patchedDllPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD byteWrite;
	WriteFile(patchedDLL, fileContent, (DWORD)fileSize, &byteWrite, nullptr);	// generate patched DLL which points to the new PDB
	CloseHandle(patchedDLL);

	// clean up
	APP_LOG_ERROR("Patching DLL succeeded!!!.\n", true);
}