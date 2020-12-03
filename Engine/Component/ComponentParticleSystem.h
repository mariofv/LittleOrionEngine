#ifndef _COMPONENTPARTICLESYSTEM_H
#define _COMPONENTPARTICLESYSTEM_H

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "Component.h"
#include "MathGeoLib.h"
#include "Main/Application.h"
#include "Module/ModuleTime.h"
#include <Helper/BezierCurve.h>
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
		float4 color;

		float particle_scale;
		float time_passed;
		float life;
		float time_counter;
		float current_sprite_x = 0, current_sprite_y = 0;
		float2 size = float2::zero;

		float4x4 model;
		float4x4 geometric_space;

		float orbit_random;
		float random_velocity_percentage;
		float random_size_percentage;
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
		VEL_CONSTANT,
		VEL_LINEAR,
		VEL_RANDOM_BETWEEN_TWO_CONSTANTS,
		VEL_CURVE
	};

	enum TypeOfSizeOverTime
	{
		SIZE_LINEAR,
		SIZE_RANDOM_BETWEEN_TWO_CONSTANTS,
		SIZE_CURVE
	};

	enum TypeOfSizeColorChange
	{
		COLOR_NONE,
		COLOR_LINEAR,
		COLOR_CURVE
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

	void ReassignResource() override;
	Component* Clone(GameObject* owner, bool original_prefab) override;
	void CopyTo(Component* component_to_copy) const override;

	ComponentParticleSystem(const ComponentParticleSystem& component_to_copy) = default;
	ComponentParticleSystem(ComponentParticleSystem&& component_to_move) = default;

	ComponentParticleSystem& operator=(const ComponentParticleSystem & component_to_copy) = default;
	ComponentParticleSystem& operator=(ComponentParticleSystem && component_to_move) = default;

	//Script methods
	ENGINE_API void Emit(size_t count);
	ENGINE_API void Play();
	ENGINE_API void Stop();
	ENGINE_API void Pause();

	void CalculateGravityVector();

	ENGINE_API bool IsPlaying() const;
	ENGINE_API bool IsEmitting() const;
	ENGINE_API bool HasParticlesAlive() const;

	bool HasToDrawParticleSystem() const;

private:
	unsigned int GetParticlesSystemVariation();
	bool emitting = true;
	bool playing = true;
	size_t number_emited = 0;

public:

	uint32_t texture_uuid;
	ComponentBillboard* billboard;
	static const int DEFAULT_TEXTURE_UUID = 3665414218;

	TypeOfParticleSystem type_of_particle_system = BOX;

	std::vector<Particle> particles;

	//Basic values
	float velocity_particles_start = 1.0F;
	float gravity_modifier = 0.F;
	float4 gravity_vector;
	float velocity_factor_mod = 0.001F;
	float gravity_factor_mod = 0.000001F;

	//Spritesheet
	float max_tile_value = 0;
	float min_tile_value = 4;

	//standard values
	bool loop = true;
	unsigned int last_used_particle = 0;
	bool active = true;

	//size
	float2 particles_size = float2(0.2f);
	bool size_over_time = false;
	TypeOfSizeOverTime type_of_size_over_time = TypeOfSizeOverTime::SIZE_LINEAR;
	float min_size_of_particle = 1.0f;
	float max_size_of_particle = 1.0f;
	BezierCurve size_curve;

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
	TypeOfSizeColorChange type_of_color_change = TypeOfSizeColorChange::COLOR_NONE;
	float4 initial_color = float4::one;
	float4 color_to_fade = float4::one;;
	float color_fade_time = 1.0F;
	BezierCurve color_curve;

	//orbit
	bool orbit = false;

	bool fade = false;
	float fade_time = 1.0F;

	//Velocity over time
	bool velocity_over_time = false;
	TypeOfVelocityOverTime type_of_velocity_over_time = TypeOfVelocityOverTime::VEL_CONSTANT;
	float velocity_over_time_speed_modifier = 1.0F;
	float velocity_over_time_speed_modifier_second = 2.0F;
	BezierCurve vel_curve;

	//Runtime values
	size_t playing_particles_number = MAX_PARTICLES;
	size_t num_of_alive_particles = 0;
	int max_particles_number = MAX_PARTICLES;
	GLuint ssbo;
	GLuint shader_program;
};

#endif
