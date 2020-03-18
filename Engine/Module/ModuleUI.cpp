#include "Component/ComponentCamera.h"
#include "Component/ComponentCanvas.h"
#include "Component/ComponentUI.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "Module/ModuleWindow.h"
#include "ModuleUI.h"
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
	for (auto &canvas : canvases)
	{
		if (canvas->IsEnabled())
		{
			canvas->Render(camera);
		}
	}

	for (auto &ui : ui_elements)
	{
		ui->Render();
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

ComponentUI* ModuleUI::CreateComponentUI()
{
	ComponentUI* new_ui = new ComponentUI();
	ui_elements.push_back(new_ui);
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

//Guardar aquí todos los component canvas (crear, destruir y guardar)
//Cuando se hace el render de los canvas, añadir un render a este modulo que renderice todos los canvas
//