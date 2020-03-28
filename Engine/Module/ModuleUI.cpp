#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentImage.h"
#include "Component/ComponentUI.h"
#include "Component/ComponentText.h"
#include "Main/Globals.h"
#include "Main/Application.h"

#include "ModuleUI.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"

#include "SDL/SDL.h"


// Called before render is available
bool ModuleUI::Init()
{
	APP_LOG_SECTION("************ Module UI Init ************");

	render = SDL_CreateRenderer(App->window->window, -1, SDL_RENDERER_ACCELERATED);
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
	window_width = App->editor->scene_panel->scene_window_content_area_width;
	window_height = App->editor->scene_panel->scene_window_content_area_height;
	float4x4 projection = float4x4::D3DOrthoProjLH(0, 1, window_width, window_height);
	for (auto &canvas : canvases)
	{
		if (canvas->IsEnabled())
		{
			//canvas->Render(camera);
		}
	}

	for (auto &ui : ui_elements)
	{
		ui->Render(&projection);
	}

	for (auto &txt : ui_texts)
	{
		txt->Render();
	}
}

ComponentCanvas* ModuleUI::CreateComponentCanvas()
{
	ComponentCanvas * new_canvas = new ComponentCanvas();
	canvases.push_back(new_canvas);
	return new_canvas;
}

void ModuleUI::RemoveComponentCanvas(ComponentCanvas* canvas_to_remove)
{
	auto it = std::find(canvases.begin(), canvases.end(), canvas_to_remove);
	if (it != canvases.end())
	{
		delete *it;
		canvases.erase(it);
	}
}

ComponentUI* ModuleUI::CreateComponentUI(ComponentUI::UIType type)
{
	ComponentUI* new_ui;
	switch (type)
	{
		case ComponentUI::UIType::CANVAS:
			//new_ui = new ComponentCanvas();
			break;
		case ComponentUI::UIType::IMAGE:
			new_ui = new ComponentImage();
			break;
		case ComponentUI::UIType::TEXT:
			//new_ui = new ComponetText();
			break;
	}
	if(new_ui) 
	{
		ui_elements.push_back(new_ui);
	}
	return new_ui;
}

void ModuleUI::RemoveComponentUI(ComponentUI* ui_to_remove)
{
	auto it = std::find(ui_elements.begin(), ui_elements.end(), ui_to_remove);
	if (it != ui_elements.end())
	{
		delete *it;
		ui_elements.erase(it);
	}
}

ComponentText* ModuleUI::CreateComponentText()
{
	ComponentText* new_txt = new ComponentText();
	ui_texts.push_back(new_txt);
	return new_txt;
}

void ModuleUI::RemoveComponentText(ComponentText* txt_to_remove)
{
	auto it = std::find(ui_texts.begin(), ui_texts.end(), txt_to_remove);
	if (it != ui_texts.end())
	{
		delete *it;
		ui_texts.erase(it);
	}
}

//Guardar aqu� todos los component canvas (crear, destruir y guardar)
//Cuando se hace el render de los canvas, a�adir un render a este modulo que renderice todos los canvas
//