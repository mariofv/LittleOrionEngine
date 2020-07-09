#ifndef _MODULEEFFECTS_H_
#define _MODULEEFFECTS_H_

#include "Module.h"


class ComponentBillboard;
class ComponentParticleSystem;
class ComponentTrail;
class ComponentTrailRenderer;
class PanelConfiguration;

class GameObject;

class ModuleEffects : public Module
{

public:

	bool CleanUp() override;
	void Render();

	ComponentBillboard* CreateComponentBillboard();
	void RemoveComponentBillboard(ComponentBillboard* billboard_to_remove);

	ComponentParticleSystem* CreateComponentParticleSystem();
	void RemoveComponentParticleSystem(ComponentParticleSystem* particle_system_to_remove);

	ComponentTrail* CreateComponentTrail(GameObject* owner);
	void RemoveComponentTrail(ComponentTrail* trail_to_remove);

	ComponentTrailRenderer* CreateComponentTrailRenderer(GameObject* owner);
	void RemoveComponentTrailRenderer(ComponentTrailRenderer* trail_renderer_to_remove);

private:
	bool render_particles = true;
	std::vector<ComponentBillboard*> billboards;
	std::vector<ComponentBillboard*> billboards_to_render;

	std::vector<ComponentParticleSystem*> particle_systems;
	std::vector<ComponentParticleSystem*> particle_systems_to_render;

	std::vector<ComponentTrail*> trails;
	std::vector<ComponentTrailRenderer*> trail_renderers;
	friend PanelConfiguration;
};

#endif