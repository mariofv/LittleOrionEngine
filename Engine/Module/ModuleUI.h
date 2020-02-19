#ifndef _MODULEUI_H_
#define _MODULEUI_H_

#include "Module.h"
#include "Main/Globals.h"

class ModuleUI : public Module
{
public:
	ModuleUI() = default;
	~ModuleUI() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;
};

#endif //_MODULEUI_H_