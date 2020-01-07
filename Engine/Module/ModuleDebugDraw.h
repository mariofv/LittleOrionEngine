#ifndef _MODULEDEBUGDRAW_H_
#define _MODULEDEBUGDRAW_H_

#include "Module.h"

#include <GL/glew.h>

class IDebugDrawOpenGLImplementation;
class ComponentCamera;

class ModuleDebugDraw : public Module
{
public:
	ModuleDebugDraw() = default;
	~ModuleDebugDraw() = default;

	bool Init() override;
	bool CleanUp() override;

	void Render(const ComponentCamera& camera);

private:
	static IDebugDrawOpenGLImplementation* dd_interface_implementation;

};

#endif //_MODULEDEBUGDRAW_H_