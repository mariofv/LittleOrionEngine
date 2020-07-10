#ifndef __PANELPARTICLESYSTEM_H__
#define __PANELPARTICLESYSTEM_H__

#include "EditorUI/Panel/Panel.h"

class ComponentParticleSystem;

class PanelParticleSystem : public Panel
{
public:
	PanelParticleSystem();
	~PanelParticleSystem() = default;

	void Render(ComponentParticleSystem* particle_system);

private:
	bool modified_by_user = false;
};

#endif //__PANELPARTICLESYSTEM_H__