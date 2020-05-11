#include "ModuleUI.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentCanvasRenderer.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentProgressBar.h"
#include "Component/ComponentText.h"
#include "Component/ComponentButton.h"

#include "Main/Globals.h"
#include "Main/GameObject.h"

#include <algorithm>
#include <Brofiler/Brofiler.h>
#include <GL/glew.h>
#include <SDL/SDL.h>


// Called before render is available
bool ModuleUI::Init()
{
	APP_LOG_SECTION("************ Module UI Init ************");

	return true;
}

// Called every draw update
update_status ModuleUI::Update()
{
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleUI::CleanUp()
{
	return true;
}

void ModuleUI::Render(const ComponentCamera* camera)
{
	BROFILER_CATEGORY("UI: Module Render", Profiler::Color::LightSeaGreen);
	if (main_canvas != nullptr)
	{
		main_canvas->Render();
	}
}

ComponentCanvas* ModuleUI::CreateComponentCanvas()
{
	ComponentCanvas* new_canvas = new ComponentCanvas();
	canvases.push_back(new_canvas);
	if (main_canvas == nullptr)
	{
		main_canvas = new_canvas;
	}
	return new_canvas;
}


void ModuleUI::RemoveComponentCanvas(ComponentCanvas* component_canvas)
{
	const auto it = std::find(canvases.begin(), canvases.end(), component_canvas);
	if (*it == main_canvas)
	{
		main_canvas = nullptr;
	}
	if (it != canvases.end())
	{
		delete *it;
		canvases.erase(it);
	}
}

GameObject* ModuleUI::GetMainCanvasGameObject() const
{
	if (main_canvas != nullptr)
	{
		return main_canvas->owner;
	}
}

void ModuleUI::RemoveComponentUI(Component* component_ui)
{
	const auto it = std::find(ui_elements.begin(), ui_elements.end(), component_ui);
	if (it != ui_elements.end())
	{
		delete *it;
		ui_elements.erase(it);
	}
}

ComponentCanvasRenderer* ModuleUI::CreateComponentCanvasRenderer()
{
	ComponentCanvasRenderer* new_canvas_renderer = new ComponentCanvasRenderer();
	canvas_renderers.push_back(new_canvas_renderer);
	return new_canvas_renderer;
}

void ModuleUI::RemoveComponentCanvasRenderer(ComponentCanvasRenderer* component_canvas_renderer)
{
	const auto it = std::find(canvas_renderers.begin(), canvas_renderers.end(), component_canvas_renderer);
	if (it != canvas_renderers.end())
	{
		delete *it;
		canvas_renderers.erase(it);
	}
}