#ifndef _MODULERESOURCEMANAGER_H_
#define _MODULERESOURCEMANAGER_H_

#include "Module.h"

class Application;

class ModuleResourceManager : public Module
{
public:

	ModuleResourceManager() = default;

	bool Init() override;

	update_status PreUpdate() override;
	bool CleanUp() override;
};

#endif // _MODULERESOURCEMANAGER_H_