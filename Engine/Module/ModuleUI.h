#ifndef _MODULEUI_H_
#define _MODULEUI_H_

#include "Module.h"
#include "Main/Globals.h"
#include "Helper/Quad.h"

#include <iostream>
#include <map>
#include <vector>

class Component;
class ComponentButton;
class ComponentCanvas;
class ComponentCanvasRenderer;
class ComponentEventSystem;
class ComponentText;
class GameObject;

class ModuleUI : public Module
{
public:
	ModuleUI() = default;
	~ModuleUI() = default;

	bool Init() override;
	update_status Update() override;

	void Render(float width, float height, bool scene_mode);

	ComponentEventSystem* CreateComponentEventSystem();
	void RemoveComponentEventSystem(ComponentEventSystem* component_event_system);
	bool ExistEventSystem() const;

	ComponentCanvas* CreateComponentCanvas();
	void RemoveComponentCanvas(ComponentCanvas* component_canvas);

	GameObject* GetMainCanvasGameObject() const;
	void SelectMainCanvas();

	template<typename T>
	T* CreateComponentUI()
	{
		T* new_component_ui = new T();
		ui_elements.push_back(new_component_ui);
		return new_component_ui;
	}
	void RemoveComponentUI(Component* component_ui);

	ComponentCanvasRenderer* CreateComponentCanvasRenderer();
	void RemoveComponentCanvasRenderer(ComponentCanvasRenderer* component_canvas_renderer);

public:
	ComponentCanvas* main_canvas = nullptr;
	bool disable_ui_render = false;

	std::unique_ptr<Quad> quad = nullptr;
private:
	std::vector<ComponentEventSystem*> event_systems;
	std::vector<ComponentCanvas*> canvases;

	std::vector<Component*> ui_elements;
	std::vector<ComponentCanvasRenderer*> canvas_renderers;
};

#endif //_MODULEUI_H_
