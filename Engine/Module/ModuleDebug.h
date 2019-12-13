#ifndef _MODULEDEBUG_H_
#define _MODULEDEBUG_H_

#include "Module.h"

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
	
	void CreateHousesRandom() const;
	
	void ShowDebugWindow();

public:
	bool show_bounding_boxes = true;
	bool show_grid = false;
	bool show_camera_frustum = true;
	bool show_quadtree = true;

	bool frustum_culling = false;
	bool quadtree_culling = false;
	float rendering_time = 0;

private:
	int num_houses = 20;
	int max_dispersion_x = 40;
	int max_dispersion_z = 40;
};

#endif //_MODULEDEBUG_H_