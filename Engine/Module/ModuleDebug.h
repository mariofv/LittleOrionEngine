#ifndef _MODULEDEBUG_H_
#define _MODULEDEBUG_H_

#include "Module.h"
#include "Component/ComponentCamera.h"
#include "GeometryRenderer.h"

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
	
	void Render(const ComponentCamera& camera) const;
	void RenderGrid(const ComponentCamera &camera) const;
	void ShowDebugWindow();

public:
	GeometryRenderer* geometry_renderer = nullptr;

private:
	bool show_grid = false;
	bool show_camera_frustum = true;
	bool show_quadtree = true;
};

#endif //_MODULEDEBUG_H_