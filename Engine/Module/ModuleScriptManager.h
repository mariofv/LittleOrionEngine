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
	void CreatePDB();
	void GetCurrentPath();

	size_t CStrlastIndexOfChar(const char * str, char findChar);

	bool patchFileName(char * fileName);

	bool FileIOisExist(const char * fileName);

	bool FileIOcopy(const char * fromFile, const char * toFile, bool overwriteExisting);

	bool patchDLL(const char * dllPath, const char patchedDllPath[MAX_PATH], char patchedPdbPath[MAX_PATH]);

private:
	HINSTANCE gameplay_dll;
	std::string working_directory;
	std::unique_ptr<File> dll_file = nullptr;
	long last_timestamp;
	long init_timestamp;

public:

	std::vector<ComponentScript*> scripts;
	//const std::unordered_map<std::string, ScriptFunction> script_map;
	std::vector<std::string> scripts_list;


};

#endif //_MODULEMANAGERSCRIPT_H_