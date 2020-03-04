#ifndef _MODULEAI_H_
#define _MODULEAI_H_

#include "Module/Module.h"
#include "AI/NavMesh.h"

class ModuleAI : public Module
{
public:
	ModuleAI() = default;
	~ModuleAI() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

private:
	NavMesh nav_mesh = NavMesh();
};

#endif //_MODULEAI_H_