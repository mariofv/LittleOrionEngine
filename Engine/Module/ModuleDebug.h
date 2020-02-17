#ifndef _MODULEDEBUG_H_
#define _MODULEDEBUG_H_

#include "Module.h"

#include <GL/glew.h>

class ModuleDebug : public Module
{
public:
	enum class CullingMode
	{
		NONE,
		FRUSTUM_CULLING,
		QUADTREE_CULLING,
		OCTTREE_CULLING,
		AABBTREE_CULLING,
		COMBINED_CULLING
	};

	ModuleDebug() = default;
	~ModuleDebug() = default;

	bool Init() override;
	bool CleanUp() override;
	
	void CreateHousesRandom() const;
	
public:
	bool show_bounding_boxes = false;
	bool show_global_bounding_boxes = false;
	bool show_grid = true;
	bool show_camera_frustum = true;
	bool show_quadtree = false;
	bool show_aabbtree = false;

	bool culling_scene_mode = false;
	CullingMode culling_mode = CullingMode::QUADTREE_CULLING;
	float rendering_time = 0;

private:
	int num_houses = 20;
	int max_dispersion_x = 40;
	int max_dispersion_z = 40;

	friend class PanelDebug;
};

#endif //_MODULEDEBUG_H_