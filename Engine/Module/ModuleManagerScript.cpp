#include "ModuleManagerScript.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Component/ComponentScript.h"

bool ModuleManagerScript::Init()
{
	APP_LOG_SECTION("************ Module Manager Script ************");
	//TODO: Load all the .dll
	//TODO: fill the vector.
	AppReference();
	return true;
}

// Called every draw update
update_status ModuleManagerScript::Update()
{
	for (auto &script : scripts)
	{
		script->Update();
	}
	return update_status::UPDATE_CONTINUE;
}

bool ModuleManagerScript::CleanUp()
{
	return true;
}

void ModuleManagerScript::AppReference() const
{
	for (auto &script : scripts)
	{
		script->SetAppPointer(App);
	}
}


