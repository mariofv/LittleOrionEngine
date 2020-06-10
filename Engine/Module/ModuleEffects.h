#ifndef _MODULEEFFECTS_H_
#define _MODULEEFFECTS_H_

#include "Module.h"


class ComponentBillboard;
class ComponentParticleSystem;

class ModuleEffects : public Module
{

public:

	bool CleanUp() override;
	void Render();

	ComponentBillboard* CreateComponentBillboard();
	void RemoveComponentBillboard(ComponentBillboard* billboard_to_remove);

	ComponentParticleSystem* CreateComponentParticleSystem();
	void RemoveComponentParticleSystem(ComponentParticleSystem* particle_system_to_remove);

private:

	std::vector<ComponentBillboard*> billboards;
	std::vector<ComponentBillboard*> billboards_to_render;

	std::vector<ComponentParticleSystem*> particle_systems;
	std::vector<ComponentParticleSystem*> particle_systems_to_render;
};

#endif