#ifndef _MODULEEDITOR_H_
#define _MODULEEDITOR_H_

#include "Module.h"
#include "Globals.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor() = default;
	~ModuleEditor() = default;

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

};

#endif //_MODULEEDITOR_H_