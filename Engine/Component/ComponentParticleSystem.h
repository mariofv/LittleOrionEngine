#ifndef _COMPONENTPARTICLESYSTEM_H
#define _COMPONENTPARTICLESYSTEM_H

#include "Component.h"
#include "MathGeoLib.h"
#include "Main/Application.h"
#include "Module/ModuleTime.h"

class GameObject;
class ComponentBillboard;

class ComponentParticleSystem : public Component
{
public:
	struct Particle {
		float3 position;
		float3 velocity;
		float4 color;
		float time_counter;
		float  life;
	};

	ComponentParticleSystem();
	~ComponentParticleSystem() = default;

	ComponentParticleSystem(GameObject* owner);
	

	void Init();
	unsigned int FirstUnusedParticle();
	void RespawnParticle(Particle& particle);
	void Render();
	void SetParticleTexture(uint32_t texture_uuid);
	void UpdateParticle();
	float3 GetPositionOfParticle();

	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;
	//Copy and move
	
	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	
public:

	uint32_t texture_uuid;
	ComponentBillboard* billboard;

	std::vector<Particle> particles;
		
	float velocity_particles = 1.0F;

	float time_counter = 0.0F;
	float time_between_particles = 0.2F;
	float particles_life_time = 3.0F;

	int max_particles = 50;

	unsigned int last_used_particle = 0;
	unsigned int nr_new_particles = 2;

	bool enabled_random_x = true;
	int max_range_random_x = 100;
	int min_range_random_x = -100;
	int position_x = 0;

	bool enabled_random_z = true;
	int max_range_random_z = 100;
	int min_range_random_z = -100;
	int position_z = 0;

	bool loop = true;
	bool color_fade = false;
};

#endif