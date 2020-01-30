#include "ModuleResourceManager.h"
bool ModuleResourceManager::Init()
{
	return true;
}

update_status ModuleResourceManager::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

 bool ModuleResourceManager::CleanUp()
{
	return true;
}