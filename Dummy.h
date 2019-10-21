#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleDummy : public Module
{
	bool Init()
	{
		LOG("Dummy Init!");
		return true;
	}

	bool CleanUp()
	{
		LOG("Dummy CleanUp!");
		return true;
	}
};