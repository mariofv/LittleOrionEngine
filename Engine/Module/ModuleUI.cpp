#include "ModuleUI.h"

#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentProgressBar.h"
#include "Component/ComponentUI.h"
#include "Component/ComponentText.h"
#include "Component/ComponentButton.h"

#include "Main/Globals.h"

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

ComponentUI* ModuleUI::CreateComponentUI(Component::ComponentType type)
{
	ComponentUI* new_component_ui = nullptr;
	switch (type)
	{
		case Component::ComponentType::UI_IMAGE:
			new_component_ui = new ComponentImage();
			break;
		case Component::ComponentType::UI_TEXT:
			new_component_ui = new ComponentText();
			break;
		case Component::ComponentType::UI_BUTTON:
			new_component_ui = new ComponentButton();
			break;
		case Component::ComponentType::UI_PROGRESS_BAR:
			new_component_ui = new ComponentProgressBar();
			break;
	}

	if(new_component_ui)
	{
		ui_elements.push_back(new_component_ui);
		SortComponentsUI();
	}

	return new_component_ui;
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

void ModuleUI::RemoveComponentUI(ComponentUI* component_ui)
{
	const auto it = std::find(ui_elements.begin(), ui_elements.end(), component_ui);
	if (it != ui_elements.end())
	{
		delete *it;
		ui_elements.erase(it);
	}

	SortComponentsUI();
}

void ModuleUI::SortComponentsUI()
{
	ordered_ui = ui_elements;
	std::sort(ordered_ui.begin(), ordered_ui.end(), [](ComponentUI* left, ComponentUI* right)
	{
		return left->layer < right->layer;
	}
	);
}