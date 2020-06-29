#ifndef _COMPONENTPARTICLESYSTEM_H
#define _COMPONENTPARTICLESYSTEM_H

#include "Component.h"
#include "MathGeoLib.h"
#include "Main/Application.h"
#include "Module/ModuleTime.h"

class GameObject;
class ComponentBillboard;

const int MAX_PARTICLES = 500;

class ComponentParticleSystem : public Component
{
public:
	struct Particle {
		float3 position_initial;
		float3 position;
		float3 velocity_initial;
		float4 color;
		Quat rotation;
		float particle_scale;
		float time_passed;
		float life;
		float time_counter;
		float current_sprite_x = 0, current_sprite_y = 0;
		float current_width = 0, current_height = 0;
	};
	enum TypeOfParticleSystem
	{
		SPHERE,
		BOX,
		CONE
	};
	enum TypeOfVelocityOverTime
	{
		CONSTANT,
		LINEAR,
		//CURVE,
		RANDOM_BETWEEN_TWO_CONSTANTS
	};
	ComponentParticleSystem();
	~ComponentParticleSystem();

	ComponentParticleSystem(GameObject* owner);
	

	void Init();
	unsigned int FirstUnusedParticle();
	void RespawnParticle(Particle& particle);
	void Render();
	void UpdateParticle(Particle& particle);
	void SetParticleTexture(uint32_t texture_uuid);

	void Delete() override;

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;
	//Copy and move
	
	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	ComponentParticleSystem(const ComponentParticleSystem& component_to_copy) = default;
	ComponentParticleSystem(ComponentParticleSystem&& component_to_move) = default;

	ComponentParticleSystem& operator=(const ComponentParticleSystem & component_to_copy) = default;
	ComponentParticleSystem& operator=(ComponentParticleSystem && component_to_move) = default;

	//Script methods
	ENGINE_API void Emit(size_t count);
	ENGINE_API void Play();
	ENGINE_API void Stop();
	ENGINE_API void Pause();

public:

	uint32_t texture_uuid;
	ComponentBillboard* billboard;

	TypeOfParticleSystem type_of_particle_system = BOX;

	std::vector<Particle> particles;

	//Basic values
	float velocity_particles_start = 1.0F;
	bool gravity = false;
	float gravity_modifier = -0.2F;

	//Spritesheet
	float max_tile_value = 0;
	float min_tile_value = 4;

	//standard values
	bool loop = true;
	unsigned int last_used_particle = 0;
	unsigned int nr_new_particles = 2;
	bool active = true;

	//size
	int min_size_of_particle = 2;
	int max_size_of_particle = 10;
	float particles_width = 0.2F;
	float particles_height = 0.2F;
	bool size_random = false;
	bool tile_random = false;
	bool change_size = false;
	float size_change_speed = 1.0F;

	//time
	float time_counter = 0.0F;
	float time_between_particles = 0.2F;
	float particles_life_time = 5.0F;

	bool follow_owner = false;

	//position X
	bool enabled_random_x = true;
	int max_range_random_x = 100;
	int min_range_random_x = -100;
	int position_x = 0;

	//position Y
	bool enabled_random_z = true;
	int max_range_random_z = 100;
	int min_range_random_z = -100;
	int position_z = 0;

	//Cone properties
	float inner_radius = 1.0F;
	float outer_radius = 3.0F;
	
	//color
	float color_particle[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	bool fade = false;
	float fade_time = 1.0F;
	float color_fade_time = 1.0F;
	bool fade_between_colors = false;
	float color_to_fade[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	//Velocity over time
	bool velocity_over_time = false;
	TypeOfVelocityOverTime type_of_velocity_over_time = RANDOM_BETWEEN_TWO_CONSTANTS;
	float velocity_over_time_speed_modifier = 1.0F;
	float velocity_over_time_speed_modifier_second = 2.0F;
	float acceleration = 0.0F;
	float3 velocity_over_time_acceleration;
	
	//Runtime values
	int playing_particles_number = MAX_PARTICLES;
	int max_particles_number = MAX_PARTICLES;
	bool playing = true;
};

#endif