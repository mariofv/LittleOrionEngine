#ifndef _MODULEDEBUG_H_
#define _MODULEDEBUG_H_

#include "Module.h"
#include "Component/ComponentCamera.h"

#include <GL/glew.h>

class ModuleDebug : public Module
{
public:
	ModuleDebug() = default;
	~ModuleDebug() = default;

	bool Init();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();
	
	void RenderGrid(const ComponentCamera &camera) const;
	void ShowDebugWindow();

};

#endif //_MODULEDEBUG_H_