#ifndef _MODULEMANAGERSCRIPT_H_
#define _MODULEMANAGERSCRIPT_H_

#include "Module.h"
#include "Main/Globals.h"
#include "Main/GameObject.h"
#include <vector>

class ComponentScript;
class GameObject;

class ModuleManagerScript : public Module
{
public:
	ModuleManagerScript() = default;
	~ModuleManagerScript() = default;

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	bool CleanUp() override;
	ComponentScript* CreateComponentScript();
public:

	std::vector<ComponentScript*> scripts;

private:
	HINSTANCE gp_dll;
};

#endif //_MODULEMANAGERSCRIPT_H_