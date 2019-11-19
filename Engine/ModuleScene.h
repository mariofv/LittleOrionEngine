#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include "Globals.h"

class ModuleScene : public Module
{
public:
	ModuleScene() = default;
	~ModuleScene();

	void ShowSceneWindow();

public:
	bool is_hovered = false;
};

#endif // _MODULSESCENE_H