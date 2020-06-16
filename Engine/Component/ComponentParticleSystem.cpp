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

ComponentParticleSystem::ComponentParticleSystem(GameObject* owner) : Component(owner, ComponentType::PARTICLE_SYSTEM)
{
	Init();
}
ComponentParticleSystem::~ComponentParticleSystem()
{
	delete billboard;
}

void ComponentParticleSystem::Init() 
{
	particles.reserve(max_particles);

	billboard = new ComponentBillboard(this->owner);
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

	if (!follow_owner)
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
void ComponentParticleSystem::UpdateParticle(Particle& particle)
{
	float time_spend = particle.time_passed;
	particle.life -= App->time->real_time_delta_time; // reduce life
	time_spend -= particle.life;
	particle.position += particle.velocity * App->time->real_time_delta_time;

	//alpha fade

	if (fade)
	{
		particle.color.w -= App->time->real_time_delta_time * (fade_time / 1000);
		billboard->color[3] = particle.color.w;
	}
	else
	{
		billboard->color[3] = 1.0F;
	}

	//fade color
	if (fade_between_colors)
	{
		float time = (time_spend / 1000) * (color_fade_time / 100);
		float temp_color[3] = { particle.color.x ,particle.color.y ,particle.color.z };
		particle.color.x = (1 - time) * particle.color.x + time * color_to_fade[0];
		particle.color.y = (1 - time) * particle.color.y + time * color_to_fade[1];
		particle.color.z = (1 - time) * particle.color.z + time * color_to_fade[2];
	}
	billboard->color[0] = particle.color.x;
	billboard->color[1] = particle.color.y;
	billboard->color[2] = particle.color.z;

	//size
	billboard->width = particles_width * particle.particle_scale;
	billboard->height = particles_height * particle.particle_scale;
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

void ComponentParticleSystem::SpecializedSave(Config& config) const
{

	config.AddInt(static_cast<int>(type_of_particle_system), "Type of particle system");
	config.AddBool(loop, "Loop");
	config.AddInt(max_particles, "Max Particles");
	config.AddInt(last_used_particle, "Last used particle");
	config.AddInt(nr_new_particles, "Number of new particles");
	config.AddBool(active, "Active");
	config.AddInt(min_size_of_particle, "Max Size Particles");
	config.AddInt(max_size_of_particle, "Min Size Particles");
	config.AddFloat(particles_width, "Particle Width");
	config.AddFloat(particles_height, "Particle Height");
	config.AddBool(size_random, "Size random");

	config.AddFloat(velocity_particles, "Velocity of particles");

	config.AddFloat(time_counter, "Time Counter");
	config.AddFloat(time_between_particles, "Time Between Particles");
	config.AddFloat(particles_life_time, "Particles Life Time");

	config.AddBool(follow_owner, "Follow owner");

	config.AddBool(enabled_random_x, "Random X position");
	config.AddInt(max_range_random_x, "Max range position x");
	config.AddInt(min_range_random_x, "Min range position x");
	config.AddInt(position_x, "Position X");

	config.AddBool(enabled_random_z, "Random Z position");
	config.AddInt(max_range_random_z, "Max range position z");
	config.AddInt(min_range_random_z, "Min range position z");
	config.AddInt(position_z, "Position Z");

	config.AddFloat(inner_radius, "Inner Radius");
	config.AddFloat(outer_radius, "Outer Radius");

	config.AddFloat(color_particle[0], "Color Particle R");
	config.AddFloat(color_particle[1], "Color Particle G");
	config.AddFloat(color_particle[2], "Color Particle B");
	config.AddFloat(color_particle[3], "Color Particle A");
	config.AddBool(fade, "Fade");
	config.AddFloat(fade_time, "Fade Time");
	config.AddFloat(color_fade_time, "Color Fade Time");
	config.AddBool(fade_between_colors, "Fade between Colors");
	config.AddFloat(color_to_fade[0], "Color to fade R");
	config.AddFloat(color_to_fade[1], "Color to fade G");
	config.AddFloat(color_to_fade[2], "Color to fade B");
	config.AddFloat(color_to_fade[3], "Color to fade A");
	billboard->SpecializedSave(config);
}

void ComponentParticleSystem::SpecializedLoad(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	type_of_particle_system = static_cast<TypeOfParticleSystem>(config.GetInt("Type of particle system", static_cast<int>(TypeOfParticleSystem::BOX)));
	
	loop = config.GetBool("Loop", true);
	max_particles = config.GetInt("Max Particles", 500);
	last_used_particle = config.GetInt("Last used particle", 0);
	nr_new_particles = config.GetInt("Number of new particles",2);
	min_size_of_particle = config.GetInt("Max Size Particles", 10);
	max_size_of_particle = config.GetInt("Min Size Particles", 2);
	particles_width = config.GetFloat("Particle Width", 0.2F);
	particles_height = config.GetFloat("Particle Height", 0.2F);
	size_random = config.GetBool("Size random", false);

	velocity_particles = config.GetFloat("Velocity of particles", 1.0F);

	time_counter = config.GetFloat("Time Counter", 0.0F);
	time_between_particles = config.GetFloat("Time Between Particles", 0.2F);
	particles_life_time = config.GetFloat("Particles Life Time", 3.0F);

	follow_owner = config.GetBool("Follow owner", false);

	enabled_random_x = config.GetBool("Random X position", true);
	max_range_random_x = config.GetInt("Max range position x", 100);
	min_range_random_x = config.GetInt("Min range position x", -100);
	position_x = config.GetInt("Position X", 0);

	enabled_random_z = config.GetBool("Random Z position", true);
	max_range_random_z = config.GetInt("Max range position z", 100);
	min_range_random_z = config.GetInt("Min range position z",-100);
	position_z = config.GetInt("Position Z", 0);

	inner_radius = config.GetFloat("Inner Radius", 1.0F);
	outer_radius = config.GetFloat("Outer Radius", 3.0F);

	color_particle[0] = config.GetFloat("Color Particle R", 1.0F);
	color_particle[1] = config.GetFloat( "Color Particle G", 1.0F);
	color_particle[2] = config.GetFloat("Color Particle B", 1.0F);
	color_particle[3] = config.GetFloat("Color Particle A", 1.0F);
	fade = config.GetBool("Fade", false);
	fade_time = config.GetFloat("Fade Time", 1.0F);
	color_fade_time = config.GetFloat("Color Fade Time", 1.0F);
	fade_between_colors = config.GetBool("Fade between Colors", false);
	color_to_fade[0] = config.GetFloat("Color to fade R", 1.0F);
	color_to_fade[1] = config.GetFloat("Color to fade G", 1.0F);
	color_to_fade[2] = config.GetFloat("Color to fade B", 1.0F);
	color_to_fade[3] = config.GetFloat("Color to fade A", 1.0F);
}

Component* ComponentParticleSystem::Clone(bool original_prefab) const
{
	ComponentParticleSystem* created_component;
	return created_component;
};

void ComponentParticleSystem::Copy(Component * component_to_copy) const
{
	
}