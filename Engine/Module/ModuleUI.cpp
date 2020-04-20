#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentProgressBar.h"
#include "Component/ComponentUI.h"
#include "Component/ComponentText.h"
#include "Component/ComponentButton.h"

#include "GL/glew.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "Main/GameObject.h"

#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"

#include "Brofiler/Brofiler.h"
#include <algorithm>
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
#if GAME
	window_width = App->window->GetWidth();
	window_height = App->window->GetHeight();
#else
	window_width = App->editor->scene_panel->scene_window_content_area_width;
	window_height = App->editor->scene_panel->scene_window_content_area_height;
#endif
	float4x4 projection = float4x4::D3DOrthoProjLH(-1, MAX_NUM_LAYERS, window_width, window_height);
	if (main_canvas != nullptr)
	{
		glDisable(GL_DEPTH_TEST);
		RenderUIGameObject(main_canvas->owner, &projection);
		glEnable(GL_DEPTH_TEST);
	}
}

void ModuleUI::RenderUIGameObject(GameObject* parent, float4x4* projection)
{
	for (auto& ui_element : ordered_ui)
	{
		if (ui_element->GetType() == Component::ComponentType::UI_IMAGE)
		{
			static_cast<ComponentImage*>(ui_element)->Render(projection);
		}
	}
}

ComponentUI* ModuleUI::CreateComponentUI(Component::ComponentType type)
{
	ComponentUI* new_ui = nullptr;
	switch (type)
	{
		case Component::ComponentType::CANVAS:
			if (main_canvas == nullptr)
			{
				main_canvas = new ComponentCanvas();
			}
			new_ui = main_canvas;
			break;
		case Component::ComponentType::UI_IMAGE:
			new_ui = new ComponentImage();
			break;
		case Component::ComponentType::UI_TEXT:
			new_ui = new ComponentText();
			break;
		case Component::ComponentType::UI_BUTTON:
			new_ui = new ComponentButton();
			break;
		case Component::ComponentType::UI_PROGRESS_BAR:
			new_ui = new ComponentProgressBar();
			break;
	}

	if(new_ui) 
	{
		ui_elements.push_back(new_ui);
		SortComponentsUI();
	}

	return new_ui;
}

void ModuleUI::RemoveComponentUI(ComponentUI* ui_to_remove)
{
	const auto it = std::find(ui_elements.begin(), ui_elements.end(), ui_to_remove);
	if (*it == main_canvas)
	{
		main_canvas = nullptr;
	}
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