#include "ComponentParticleSystem.h"

#include "Main/Application.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleProgram.h"
#include "Component/ComponentBillboard.h"
#include "Module/ModuleResourceManager.h"
#include "GL/glew.h"

ComponentParticleSystem::ComponentParticleSystem() : Component(nullptr, ComponentType::PARTICLE_SYSTEM)
{
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
		ComponentBillboard* created_billboard = new ComponentBillboard(owner);
		
		created_billboard->billboard_texture = App->resources->Load<Texture>(DEFAULT_BILLBOARD_TEXTURE_PATH);
		created_billboard->alignment_type = ComponentBillboard::AlignmentType::VIEW_POINT;
		created_billboard->x_tiles = 1;
		created_billboard->y_tiles = 1;
		created_billboard->sheet_speed = 1;
		particles[i].billboard = created_billboard;
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
	//particle.Position = owner.Position + random + offset;
	particle.color = float4(rColor, rColor, rColor, 1.0f);
	particle.life = 1.0f;
	//particle.Velocity = object.Velocity * 0.1f;
}

void ComponentParticleSystem::Update()
{
	for (unsigned int i = 0; i < nr_new_particles; ++i)
	{
		int unused_particle = FirstUnusedParticle();
		RespawnParticle(particles[unused_particle]);
	}
	// update all particles
	for (unsigned int i = 0; i < max_particles; ++i)
	{
		Particle &p = particles[i];
		p.life -= self_timer.Read(); // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.position -= p.velocity * self_timer.Read();
			p.color.w -= self_timer.Read() * 2.5f;
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