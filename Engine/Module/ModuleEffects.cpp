#include "ModuleEffects.h"

#include "Component/ComponentParticleSystem.h"
#include "Component/ComponentBillboard.h"

#include "Main/GameObject.h"
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
	return false;
}

void ModuleEffects::Render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	for (auto &billboard : billboards)
	{
		billboard->Render(billboard->owner->transform.GetGlobalTranslation());
	}
	glDisable(GL_BLEND);
	for (auto &particles : particle_systems)
	{
		particles->Render();
	}
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
