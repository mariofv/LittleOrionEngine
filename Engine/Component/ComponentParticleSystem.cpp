#include "ComponentParticleSystem.h"

#include "Main/Application.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleProgram.h"

#include "Component/ComponentBillboard.h"
#include "Module/ModuleResourceManager.h"
#include "GL/glew.h"

ComponentParticleSystem::ComponentParticleSystem() : Component(nullptr, ComponentType::PARTICLE_SYSTEM)
{
	self_timer.Start();
	Init();
}

ComponentParticleSystem::ComponentParticleSystem(GameObject * owner) : Component(owner, ComponentType::PARTICLE_SYSTEM)
{
	
}

void ComponentParticleSystem::Init() 
{
	particles.reserve(max_particles);
	for (unsigned int i = 0; i < max_particles; ++i)
	{
		particles.emplace_back(Particle());
		particles[i].billboard = new ComponentBillboard(owner);
		particles[i].billboard->width = 0.5f; particles[i].billboard->height= 0.5f;

	}
}

unsigned int ComponentParticleSystem::FirstUnusedParticle()
{
	for (unsigned int i =last_used_particle; i < max_particles; ++i)
	{
		if (particles[i].life <= 0.0f) {
			last_used_particle = i;
			return i;
		}
	}
	for (unsigned int i = 0; i < last_used_particle; ++i) {
		if (particles[i].life <= 0.0f) {
			last_used_particle = i;
			return i;
		}
	}
	last_used_particle = 0;
	return 0;
}

void ComponentParticleSystem::RespawnParticle(Particle& particle)
{
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.position = owner->transform.GetGlobalTranslation();
	particle.color = float4(rColor, rColor, rColor, 1.0f);
	particle.life = 4000.0f;
	particle.velocity.y =  0.001f;
}

void ComponentParticleSystem::Render()
{

	
	// update all particles
	for (unsigned int i = 0; i < max_particles; ++i)
	{
		Particle &p = particles[i];
		p.life -= App->time->real_time_delta_time; // reduce life
		float random = (rand() % 100)/1000.f;
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			
			p.position.y +=  p.velocity.y*App->time->real_time_delta_time;
			//p.color.w -= App->time->real_time_delta_time * 2.5f;
			//fade
			p.billboard->Render(p.position);
		}
		else 
		{
			int unused_particle = FirstUnusedParticle();
			RespawnParticle(particles[unused_particle]);
		}
	}
}

void ComponentParticleSystem::Delete()
{
	App->renderer->RemoveComponentParticleSystem(this);
}

void ComponentParticleSystem::Save(Config& config) const
{

}

void ComponentParticleSystem::Load(const Config& config)
{
	
}

Component* ComponentParticleSystem::Clone(bool original_prefab) const
{
	ComponentLight * created_component;
	
	return created_component;
};

void ComponentParticleSystem::Copy(Component * component_to_copy) const
{
	
}