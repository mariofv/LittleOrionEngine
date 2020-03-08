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

//Guardar aquí todos los component canvas (crear, destruir y guardar)
//Cuando se hace el render de los canvas, añadir un render a este modulo que renderice todos los canvas
//