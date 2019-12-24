#ifndef _MODULEDEBUGDRAW_H_
#define _MODULEDEBUGDRAW_H_

#include "Module.h"

#include <GL/glew.h>

class IDebugDrawOpenGLImplementation;

class ModuleDebugDraw : public Module
{
public:
	ModuleDebugDraw() = default;
	~ModuleDebugDraw() = default;

	bool Init();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

private:
	static IDebugDrawOpenGLImplementation* dd_interface_implementation;

};

#endif //_MODULEDEBUGDRAW_H_