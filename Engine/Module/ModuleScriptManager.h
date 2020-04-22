#ifndef _MODULEMANAGERSCRIPT_H_
#define _MODULEMANAGERSCRIPT_H_

#include "Main/Globals.h"
#include "Module.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <Helper/cr.h>

class ComponentScript;
class GameObject;
class Script;
class Path;
class Config;


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
	void ReLink();

	void SaveVariables(std::unordered_map<uint64_t, Config>& config_list);
	void LoadVariables(std::unordered_map<uint64_t, Config> config_list);

private:
	void GetCurrentPath();

	void LoadScriptList();
	void SaveScriptList();
	void RunScripts();
	void RemoveScriptPointers();

	void InitDLL();
	void ReloadDLL();

public:
	std::vector<ComponentScript*> scripts;
	std::vector<std::string> scripts_list;

private:

	cr_plugin ctx;
	HINSTANCE gameplay_dll;

	std::string working_directory;

	Path* dll_file = nullptr;
	Path* scripts_list_file_path = nullptr;

	long last_timestamp_dll;
	long init_timestamp_dll;
	long last_timestamp_script_list;
	long init_timestamp_script_list;

};

#endif //_MODULEMANAGERSCRIPT_H_