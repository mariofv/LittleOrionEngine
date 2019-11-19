#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include "Globals.h"

class ModuleScene : public Module
{
public:
	ModuleScene() = default;
	~ModuleScene();

	void ShowSceneWindow() const;
};

#endif // _MODULSESCENE_H