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
	billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::CROSSED);

	for (unsigned int i = 0; i < max_particles; ++i)
	{
		particles.emplace_back(Particle());
		particles[i].life = 0.0F;
		particles[i].particle_scale = 1.0F;
		particles[i].time_passed = particles[i].life;
	}
	
}

unsigned int ComponentParticleSystem::FirstUnusedParticle()
{
	for (unsigned int i =last_used_particle; i < max_particles; ++i)
	{
		if (particles[i].life <= 0.0f) 
		{
			last_used_particle = i;
			return i;
		}
	}
	for (unsigned int i = 0; i < last_used_particle; ++i) 
	{
		if (particles[i].life <= 0.0f) 
		{
			last_used_particle = i;
			return i;
		}
	}
	last_used_particle = 0;
	return 0;
}

void ComponentParticleSystem::RespawnParticle(Particle& particle)
{
	particle.position = float3(0.0f, 0.0f, 0.0f);
	particle.rotation = owner->transform.GetGlobalRotation();

	float scale = (rand() % ((max_size_of_particle - min_size_of_particle) + 1) + min_size_of_particle) / 100.f;

	if (size_random)
	{
		particle.particle_scale = scale;
	}
	else 
	{
		particle.particle_scale = 1.0f;
	}

	switch (type_of_particle_system)
	{
		case SPHERE:
			particle.velocity = float3::RandomDir(LCG(), velocity_particles) / 1000;
		break;
		case BOX:
		{
			float random_x = (rand() % ((max_range_random_x - min_range_random_x) + 1) + min_range_random_x) / 100.f;

			float random_z = (rand() % ((max_range_random_z - min_range_random_z) + 1) + min_range_random_z) / 100.f;

			particle.velocity.y = velocity_particles / 1000;
			if (enabled_random_x)
			{
				particle.position.x = random_x;
			}
			else
			{
				particle.position.x = position_x / 100.0F;
			}
			if (enabled_random_z)
			{
				particle.position.z = random_z;
			}
			else
			{
				particle.position.z = position_z / 100.0F;
			}
		
		break;
		}
		case CONE:
			float angle = ((rand() % 100)/100.f) * 2 * math::pi;
			float radius = inner_radius * sqrt((rand() % 100) / 100.f);
			float proportion = outer_radius / inner_radius;
			particle.position.x = radius * math::Cos(angle);
			particle.position.z = radius * math::Sin(angle);
			float distance = velocity_particles * particles_life_time;
			float height = sqrt((distance*distance) - ((radius*proportion) - radius)*((radius*proportion) - radius));
			float3 final_local_position = float3(particle.position.x*proportion, height, particle.position.z*proportion);
			particle.velocity = (final_local_position - particle.position);
			particle.velocity = particle.velocity.ScaledToLength(velocity_particles) / 1000;
		break;

	}
	
	particle.color = { color_particle[0], color_particle[1], color_particle[2], 1.0f };
	particle.life = particles_life_time*1000;
	particle.time_passed = particle.life;
	float4 aux_velocity(particle.velocity, 1.0F);
	aux_velocity = particle.rotation * aux_velocity;
	particle.velocity = aux_velocity.xyz();

	if (follow_owner)
	{
	}
	else
	{
		particle.position = owner->transform.GetGlobalTranslation() + (particle.rotation *particle.position);
	}
	

}

void ComponentParticleSystem::Render()
{
	glEnable(GL_BLEND);
	if (active) 
	{
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
		
			if (p.life > 0.0f)
			{
				UpdateParticle(p);
				if (follow_owner)
				{
					billboard->Render(owner->transform.GetGlobalTranslation() + (p.rotation *p.position));
				}
				else
				{
					billboard->Render(p.position);
				}
				
			}
		}
	}
	
	glDisable(GL_BLEND);
}
void ComponentParticleSystem::UpdateParticle(Particle& p)
{
	float time_spend = p.time_passed;
	p.life -= App->time->real_time_delta_time; // reduce life
	time_spend -= p.life;
	p.position += p.velocity * App->time->real_time_delta_time;

	//alpha fade

	if (fade)
	{
		p.color.w -= App->time->real_time_delta_time * (fade_time / 1000);
		billboard->color[3] = p.color.w;
	}
	else
	{
		billboard->color[3] = 1.0F;
	}

	//fade color

	if (fade_between_colors)
	{
		float time = (time_spend / 1000) * (color_fade_time / 10);
		float temp_color[3] = { p.color.x ,p.color.y ,p.color.z };
		temp_color[0] = (1 - time) * p.color.x + time * color_to_fade[0];
		temp_color[1] = (1 - time) * p.color.y + time * color_to_fade[1];
		temp_color[2] = (1 - time) * p.color.z + time * color_to_fade[2];
		billboard->color[0] = temp_color[0];
		billboard->color[1] = temp_color[1];
		billboard->color[2] = temp_color[2];
	}

	//size
	billboard->width = particles_width * p.particle_scale;
	billboard->height = particles_height * p.particle_scale;
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