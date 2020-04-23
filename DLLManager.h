#ifndef _DLLMANAGER_H_
#define _DLLMANAGER_H_

#include "Main/Globals.h"

#include <cr.h>

class Path;

class DLLManager
{
public:
	DLLManager();
	~DLLManager() = default;

	bool DLLItsUpdated();
	bool InitDLL();
	bool ReloadDLL();

public:
	HINSTANCE gameplay_dll;

private:
	cr_plugin hot_reloading_context;
	
	std::string working_directory;

	Path* dll_file = nullptr;
	Path* scripts_list_file_path = nullptr;

	long last_timestamp_dll;
	long init_timestamp_dll;
	long last_timestamp_script_list;
	long init_timestamp_script_list;
};

#endif // !_PATH_H_