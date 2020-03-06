#ifndef _MODULEMANAGERSCRIPT_H_
#define _MODULEMANAGERSCRIPT_H_

#include "Module.h"
#include "Main/Globals.h"
#include "Main/GameObject.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

struct CV_INFO_PDB70
{
	DWORD	CvSignature;
	GUID	Signature;
	DWORD	Age;
	BYTE	PdbFileName[];
};

class ComponentScript;
class GameObject;
class Script;
class File;
typedef void(*ScriptFunction)(void);
class ModuleScriptManager : public Module
{
public:
	ModuleScriptManager() = default;
	~ModuleScriptManager() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;
	void InitResourceScript();
	Script * CreateResourceScript(const std::string & script_name, GameObject * owner);
	ComponentScript* CreateComponentScript();
	void RemoveComponentScript(ComponentScript* script_to_remove);
	void LoadScriptList();
	void ReloadDLL();
	void GetCurrentPath();
	size_t CStrlastIndexOfChar(const char * str, char find_char);
	bool patchFileName(char* filename);
	bool CopyPDB(const char* from_file, const char* destination_file, bool overwrite_existing);
	bool patchDLL(const char* dll_path, const char patched_dll_path[MAX_PATH]);

private:
	HINSTANCE gameplay_dll;
	std::string working_directory;
	std::unique_ptr<File> dll_file = nullptr;
	long last_timestamp;
	long init_timestamp;

public:

	std::vector<ComponentScript*> scripts;
	std::vector<std::string> scripts_list;


};

#endif //_MODULEMANAGERSCRIPT_H_