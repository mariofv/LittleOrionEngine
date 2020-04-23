#ifndef _MODULEMANAGERSCRIPT_H_
#define _MODULEMANAGERSCRIPT_H_

#include "Main/Globals.h"
#include "Module.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class ComponentScript;
class Config;
class DLLManager;
class GameObject;
class Path;
class Script;


class ModuleScriptManager : public Module
{
public:
	ModuleScriptManager() = default;
	~ModuleScriptManager() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	ComponentScript* CreateComponentScript();
	void RemoveComponentScript(ComponentScript* script_to_remove);

	void CreateScript(const std::string& name);

	void InitResourceScript();
	Script* CreateResourceScript(const std::string& script_name, GameObject* owner);

	void InitScripts();

	void Refresh();
	void ReLink();

	void SaveVariables(std::unordered_map<uint64_t, Config>& config_list);
	void LoadVariables(std::unordered_map<uint64_t, Config> config_list);

private:

	void LoadScriptList();
	void SaveScriptList();

	void RunScripts();

	void RemoveScriptPointers();

	void ReloadDLL();

public:
	std::vector<ComponentScript*> scripts;
	std::vector<std::string> scripts_list;

private:
	DLLManager* dll = nullptr;

	Path* scripts_list_file_path = nullptr;


};

#endif //_MODULEMANAGERSCRIPT_H_