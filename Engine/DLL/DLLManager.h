#ifndef _DLLMANAGER_H_
#define _DLLMANAGER_H_

#include "Main/Globals.h"

#include <cr.h>
#include <iostream>
#include <string>
#include <unordered_map>

class Path;

class DLLManager
{
public:
	DLLManager();
	~DLLManager() = default;

	void InitFolderTimestamps();
	bool CheckFolderTimestamps();

	bool DLLItsUpdated();

	void CheckGameplayFolderStatus();
	void CompileGameplayProject() const;

	void CheckCompilation();

	bool InitDLL();
	bool ReloadDLL();
	void CleanUp();

	void CleanFolder() const;


public:
	HINSTANCE gameplay_dll;

private:
	cr_plugin hot_reloading_context;
	
	std::string working_directory;

	Path* dll_file = nullptr;
	Path* scripts_folder = nullptr;
	std::unordered_map<Path*, uint32_t> scripts_timestamp_map;

	long init_timestamp_dll;
	long last_timestamp_dll;
	long init_timestamp_script_folder;
	long last_timestamp_script_folder;

	std::vector<std::string> required_files{ "GamePlaySystem","GameplaySystemDebug","GameplaySystemRelease","TemplateScript"};

	bool compiling = false;
	bool hot_reloading = false;
};

#endif // !_PATH_H_