#ifndef _MODULEUI_H_
#define _MODULEUI_H_
#define ENGINE_EXPORTS

#include "Component/ComponentUI.h"
#include "Module.h"
#include "Main/Globals.h"

#include <iostream>
#include <map>
#include <GL/glew.h>

#define MAX_NUM_LAYERS 8

class ComponentCanvas;
class ComponentCamera;
class ComponentText;
class ComponentButton;
class Gameobject;

class ModuleUI : public Module
{
public:
	ModuleUI() = default;
	~ModuleUI() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	void Render(const ComponentCamera* camera);

	ComponentCanvas* CreateComponentCanvas();
	void RemoveComponentCanvas(ComponentCanvas* component_canvas);

	ComponentUI* CreateComponentUI(Component::ComponentType ui_component_type);
	void RemoveComponentUI(ComponentUI* component_ui);
	
	ENGINE_API void SortComponentsUI();

public:
	ComponentCanvas* main_canvas = nullptr;

private:
	std::vector<ComponentCanvas*> canvases;

	std::vector<ComponentUI*> ui_elements;
	std::vector<ComponentUI*> ordered_ui;
};

#endif //_MODULEUI_H_