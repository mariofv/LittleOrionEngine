#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleUI.h"


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

//Guardar aquí todos los component canvas (crear, destruir y guardar)
//Cuando se hace el render de los canvas, añadir un render a este modulo que renderice todos los canvas
//