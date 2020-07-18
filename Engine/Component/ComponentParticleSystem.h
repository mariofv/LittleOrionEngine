#ifndef _COMPONENTPARTICLESYSTEM_H
#define _COMPONENTPARTICLESYSTEM_H

#define ENGINE_EXPORTS

#include "Component.h"
#include "MathGeoLib.h"
#include "Main/Application.h"
#include "Module/ModuleTime.h"
#include <GL/glew.h>

class GameObject;
class ComponentBillboard;

const int MAX_PARTICLES = 500;

class ComponentParticleSystem : public Component
{
public:
	struct Particle {
		float4 position_initial;
		float4 position;

		float4 velocity_initial;
		float4 velocity;

		float4 color;

		float particle_scale;
		float time_passed;
		float life;
		float time_counter;
		float current_sprite_x = 0, current_sprite_y = 0;
		float2 size = float2::zero;

		float4x4 model;
		float4x4 geometric_space;

		float inital_random_orbit;
		float float2;
		float float3;
		float float4;
		/*
		if you add a parameter here you have to put the equivalent in the shader particles.vs
		Also you will need to add block of 4 floats, so if you add a float like this
		
		float f1;
		 
		Add 3 more even if you don't use them:

		float f2,f3,f4;
		*/
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
		RANDOM_BETWEEN_TWO_CONSTANTS,
		//CURVE
	};

	ComponentParticleSystem();
	~ComponentParticleSystem();

	ComponentParticleSystem(GameObject* owner);
	

	void Init() override;

	void Update();
	void UpdateParticle(Particle& particle);

	unsigned int FirstUnusedParticle();
	void RespawnParticle(Particle& particle);
	void Render();
	void SetParticleTexture(uint32_t texture_uuid);

	void Delete() override;
	void Disable() override;
	void Enable() override;

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

	void OrbitX(float angle, Particle& particle);

private:
	unsigned int GetParticlesSystemVariation();
	bool emitting = false;
	size_t number_emited = 0;

public:

	uint32_t texture_uuid;
	ComponentBillboard* billboard;

	TypeOfParticleSystem type_of_particle_system = BOX;

	std::vector<Particle> particles;

	//Basic values
	float velocity_particles_start = 1.0F;
	float gravity_modifier = 0.f;
	float4 gravity_vector;

	//Spritesheet
	float max_tile_value = 0;
	float min_tile_value = 4;

	//standard values
	bool loop = true;
	unsigned int last_used_particle = 0;
	bool active = true;

	//size
	float min_size_of_particle = 0.2f;
	float max_size_of_particle = 0.2f;
	float2 particles_size = float2(0.2f);
	bool size_random = false;
	bool change_size = false;
	float size_change_speed = 1.0F;

	bool tile_random = false;

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
	float4 initial_color = float4::one;
	bool fade_between_colors = false;
	float4 color_to_fade = float4::one;;
	float color_fade_time = 1.0F;

	bool fade = false;
	bool orbit = false;
	float fade_time = 1.0F;

	//Velocity over time
	bool velocity_over_time = false;
	TypeOfVelocityOverTime type_of_velocity_over_time = RANDOM_BETWEEN_TWO_CONSTANTS;
	float velocity_over_time_speed_modifier = 1.0F;
	float velocity_over_time_speed_modifier_second = 2.0F;
	float acceleration = 0.0F;
	float3 velocity_over_time_acceleration;

	
	//Runtime values
	size_t playing_particles_number = MAX_PARTICLES;
	size_t num_of_alive_particles = 0;
	int max_particles_number = MAX_PARTICLES;
	bool playing = true;
	GLuint ssbo;
	GLuint shader_program;

};

#endif