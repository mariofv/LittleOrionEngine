#include "ModuleEffects.h"

#include "Component/ComponentParticleSystem.h"
#include "Component/ComponentBillboard.h"
#include "Component/ComponentTrail.h"
#include "Component/ComponentTrailrenderer.h"

#include "Main/GameObject.h"
#include <Brofiler/Brofiler.h>

bool ModuleEffects::CleanUp()
{
	for (auto& particle : particle_systems)
	{
		particle->owner->RemoveComponent(particle);
	}

	for (auto& bilboard : billboards)
	{
		bilboard->owner->RemoveComponent(bilboard);
	}
	return true;
}

void ModuleEffects::Render()
{


	if (!render_particles)
	{
		return;
	}
	BROFILER_CATEGORY("Module Effects Render", Profiler::Color::OrangeRed);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	for (auto &billboard : billboards)
	{
		billboard->Render(billboard->owner->transform.GetGlobalTranslation());
	}

	for (auto &particles : particle_systems)
	{
		particles->Render();
	}

	for (auto &trail : trails)
	{
		trail->UpdateTrail();
	}

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	
}

ComponentBillboard* ModuleEffects::CreateComponentBillboard()
{
	ComponentBillboard* created_billboard = new ComponentBillboard();
	billboards.push_back(created_billboard);
	return created_billboard;
}

void ModuleEffects::RemoveComponentBillboard(ComponentBillboard* billboard_to_remove)
{
	auto it = std::find(billboards.begin(), billboards.end(), billboard_to_remove);
	if (it != billboards.end())
	{
		delete *it;
		billboards.erase(it);
	}
}

ComponentParticleSystem* ModuleEffects::CreateComponentParticleSystem()
{
	ComponentParticleSystem* created_particle_system = new ComponentParticleSystem();
	particle_systems.push_back(created_particle_system);
	return created_particle_system;
}

void ModuleEffects::RemoveComponentParticleSystem(ComponentParticleSystem* particle_system_to_remove)
{
	auto it = std::find(particle_systems.begin(), particle_systems.end(), particle_system_to_remove);
	if (it != particle_systems.end())
	{
		delete *it;
		particle_systems.erase(it);
	}
}

ComponentTrail* ModuleEffects::CreateComponentTrail(GameObject* owner)
{
	ComponentTrail* created_trail = new ComponentTrail(owner);
	trails.push_back(created_trail);
	return created_trail;
}

void ModuleEffects::RemoveComponentTrail(ComponentTrail* trail_to_remove)
{
	auto it = std::find(trails.begin(), trails.end(), trail_to_remove);
	if (it != trails.end())
	{
		delete *it;
		trails.erase(it);
	}
}

ComponentTrailRenderer* ModuleEffects::CreateComponentTrailRenderer(GameObject* owner)
{
	ComponentTrailRenderer* created_trail_renderer = new ComponentTrailRenderer(owner);
	trail_renderers.push_back(created_trail_renderer);
	return created_trail_renderer;
}

void ModuleEffects::RemoveComponentTrailRenderer(ComponentTrailRenderer* trail_renderer_to_remove)
{
	auto it = std::find(trail_renderers.begin(), trail_renderers.end(), trail_renderer_to_remove);
	if (it != trail_renderers.end())
	{
		delete *it;
		trail_renderers.erase(it);
	}
}
