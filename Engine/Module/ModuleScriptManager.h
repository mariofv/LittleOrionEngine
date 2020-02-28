#ifndef _MODULEMANAGERSCRIPT_H_
#define _MODULEMANAGERSCRIPT_H_

#include "Module.h"
#include "Main/Globals.h"
#include "Main/GameObject.h"
#include <vector>
#include <string>
#include <unordered_map>

class ComponentScript;
class GameObject;
class Script;
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

private:
	HINSTANCE gameplay_dll;
public:

	std::vector<ComponentScript*> scripts;
	//const std::unordered_map<std::string, ScriptFunction> script_map;
	std::vector<std::string> scripts_list;


};

#endif //_MODULEMANAGERSCRIPT_H_