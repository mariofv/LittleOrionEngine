#ifndef _MODULEMANAGERSCRIPT_H_
#define _MODULEMANAGERSCRIPT_H_

#include "Main/GameObject.h"
#include "Main/Globals.h"
#include "Module.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


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
class Utils;

class ModuleScriptManager : public Module
{
public:
	ModuleScriptManager() = default;
	~ModuleScriptManager() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	void CreateScript(const std::string& name);
	void InitResourceScript();
	Script* CreateResourceScript(const std::string& script_name, GameObject* owner);
	ComponentScript* CreateComponentScript();
	void RemoveComponentScript(ComponentScript* script_to_remove);

	void InitScripts();
	void Refresh();

private:
	void GetCurrentPath();
	long TimeStamp(const char* path);

	void LoadScriptList();
	void SaveScriptList();
	void RunScripts();
	void RemoveScriptPointers();

	void InitDLL();
	void ReloadDLL();
	bool CopyPDB(const char* from_file, const char* destination_file, bool overwrite_existing);
	bool PatchDLL(const char* dll_path, const char* patched_dll_path);

	HINSTANCE gameplay_dll;
	std::string working_directory;
	std::unique_ptr<File> dll_file = nullptr;
	std::unique_ptr<File> scripts_list_file = nullptr;
	Utils* utils = nullptr;

	long last_timestamp_dll;
	long init_timestamp_dll;
	long last_timestamp_script_list;
	long init_timestamp_script_list;

public:

	std::vector<ComponentScript*> scripts;
	std::vector<std::string> scripts_list;


};

#endif //_MODULEMANAGERSCRIPT_H_