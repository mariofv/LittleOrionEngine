#ifndef _MODULEUI_H_
#define _MODULEUI_H_

#include "Module.h"
#include "Main/Globals.h"

class ComponentCanvas;
class ComponentCamera;
class ComponentText;
class ComponentUI;

struct SDL_Renderer;

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
	void RemoveComponentCanvas(ComponentCanvas* canvas_to_remove);

	ComponentUI* CreateComponentUI();
	void RemoveComponentUI(ComponentUI*);

	ComponentText* CreateComponentText();
	void RemoveComponentText(ComponentText*);

	std::vector<ComponentCanvas*> canvases;
	std::vector<ComponentUI*> ui_elements;
	std::vector<ComponentText*> ui_texts;
private:
	SDL_Renderer* render;
};

#endif //_MODULEUI_H_