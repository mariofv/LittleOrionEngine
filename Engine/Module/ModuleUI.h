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

	ComponentUI* CreateComponentUI(const ComponentUI::UIType, GameObject*);
	void RemoveComponentUI(ComponentUI*);
	
	void InitGlyph();
	ENGINE_API void SortComponentsUI();

private:
	void RenderUIGameObject(GameObject*, float4x4*);

public:
	float window_width, window_height;
	ComponentCanvas* main_canvas = nullptr;

private:
	std::vector<ComponentUI*> ui_elements;
	std::vector<ComponentUI*> ordered_ui;

};

#endif //_MODULEUI_H_