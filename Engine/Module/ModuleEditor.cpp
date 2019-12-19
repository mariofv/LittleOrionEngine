#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"

// Called before render is available
bool ModuleEditor::Init()
{
	APP_LOG_SECTION("************ Module Editor Init ************");

	
	APP_LOG_SUCCESS("IMGUI editor initialized correctly.");

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleEditor::Update()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
	return true;
}