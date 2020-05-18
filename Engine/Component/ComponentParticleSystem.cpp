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

	billboard = new ComponentBillboard(owner);
	billboard->width = 0.2f; billboard->height = 0.2f;
	billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::CROSSED);

	for (unsigned int i = 0; i < max_particles; ++i)
	{
		particles.emplace_back(Particle());
		particles[i].life = 0.0F;
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
	float random_x = (rand() % ((max_range_random_x - min_range_random_x) + 1) + min_range_random_x) / 100.f;
	float random_z = (rand() % ((max_range_random_z - min_range_random_z) + 1) + min_range_random_z) / 100.f;
	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.position = owner->transform.GetGlobalTranslation();
	if (enabled_random_x)
	{
		particle.position.x += random_x;
	}
	else
	{
		particle.position.x += position_x /100.0F;
	}
	if (enabled_random_z)
	{
		particle.position.z += random_z;
	}
	else
	{
		particle.position.z += position_z / 100.0F;
	}
	
	particle.color = float4(rColor, rColor, rColor, 1.0f);
	particle.life = particles_life_time*1000;
	particle.velocity.y = velocity_particles/1000.F;
}

void ComponentParticleSystem::Render()
{
	glEnable(GL_BLEND);
	time_counter += App->time->real_time_delta_time;

	if (time_counter >= (time_between_particles * 1000))
	{
		if (loop)
		{
			int unused_particle = FirstUnusedParticle();
			RespawnParticle(particles[unused_particle]);
			time_counter = 0.0F;
		}	
	}
	
	// update all particles
	for (unsigned int i = 0; i < max_particles; ++i)
	{
		Particle& p = particles[i];
		p.life -= App->time->real_time_delta_time; // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.position.y += p.velocity.y*App->time->real_time_delta_time;
			billboard->Render(p.position);
		}
	}
	glDisable(GL_BLEND);
}
void ComponentParticleSystem::SetParticleTexture(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;
	billboard->ChangeTexture(texture_uuid);
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