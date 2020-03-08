#ifndef _MODULEUI_H_
#define _MODULEUI_H_

#include "Module.h"
#include "Main/Globals.h"
#include "Component/ComponentCanvas.h"

class ModuleUI : public Module
{
public:
	ModuleUI() = default;
	~ModuleUI() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	ComponentCanvas* CreateComponentCanvas();
	void RemoveComponentCanvas(ComponentCanvas* canvas_to_remove);

	std::vector<ComponentCanvas*> canvases;
};

#endif //_MODULEUI_H_