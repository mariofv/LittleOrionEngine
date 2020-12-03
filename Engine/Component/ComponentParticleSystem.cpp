#include "ComponentParticleSystem.h"

#include "Main/Application.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleEffects.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleResourceManager.h"
#include "Rendering/Viewport.h"

#include "Component/ComponentBillboard.h"
#include "GL/glew.h"

ComponentParticleSystem::ComponentParticleSystem() : Component(nullptr, ComponentType::PARTICLE_SYSTEM)
{

}

ComponentParticleSystem::ComponentParticleSystem(GameObject* owner) : Component(owner, ComponentType::PARTICLE_SYSTEM)
{

}
ComponentParticleSystem::~ComponentParticleSystem()
{
	delete billboard;
}

void ComponentParticleSystem::Init()
{
	glGenBuffers(1, &ssbo);
	particles.reserve(MAX_PARTICLES);

	billboard = new ComponentBillboard(this->owner);
	billboard->ChangeBillboardType(ComponentBillboard::AlignmentType::WORLD);
	billboard->ChangeTexture(DEFAULT_TEXTURE_UUID);

	for (unsigned int i = 0; i < MAX_PARTICLES; ++i)
	{
		particles.emplace_back(Particle());
		particles[i].life = 0.0F;
		particles[i].particle_scale = 1.0F;
		particles[i].time_counter = particles[i].life;
		particles[i].time_passed = 0.0F;
	}

	vel_curve = BezierCurve();
	size_curve = BezierCurve();
	color_curve = BezierCurve();
}

unsigned int ComponentParticleSystem::FirstUnusedParticle()
{
	for (unsigned int i =last_used_particle; i < max_particles_number; ++i)
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
	particle.position_initial = float4(0.0f, 0.0f, 0.0f,0.0f);

	if (tile_random)
	{
		particle.current_sprite_x = (rand() % static_cast<int>((max_tile_value - min_tile_value) + 1) + min_tile_value);
		particle.current_sprite_y = (rand() % static_cast<int>((max_tile_value - min_tile_value) + 1) + min_tile_value);
	}

	switch (type_of_particle_system)
	{
		case SPHERE:
			particle.velocity_initial = float4(float3::RandomDir(LCG(), 1), 0.0f);
		break;
		case BOX:
		{
			float random_x = (rand() % ((max_range_random_x - min_range_random_x) + 1) + min_range_random_x) / 100.f;

			float random_z = (rand() % ((max_range_random_z - min_range_random_z) + 1) + min_range_random_z) / 100.f;

			particle.velocity_initial = float4::unitY;
			if (enabled_random_x)
			{
				particle.position_initial.x = random_x;
			}
			else
			{
				particle.position_initial.x = position_x / 100.0F;
			}
			if (enabled_random_z)
			{
				particle.position_initial.z = random_z;
			}
			else
			{
				particle.position_initial.z = position_z / 100.0F;
			}

		break;
		}
		case CONE:
			float angle = ((rand() % 100)/100.f) * 2 * math::pi;
			float radius = inner_radius * sqrt((rand() % 100) / 100.f);
			float proportion = outer_radius / inner_radius;
			particle.position_initial.x = radius * math::Cos(angle);
			particle.position_initial.z = radius * math::Sin(angle);
			float distance = velocity_particles_start * particles_life_time;
			float height = sqrt((distance*distance) - ((radius*proportion) - radius)*((radius*proportion) - radius));
			float4 final_local_position = float4(particle.position_initial.x*proportion, height, particle.position_initial.z*proportion,0.0f);
			particle.velocity_initial = (final_local_position - particle.position_initial);
		break;
	}

	particle.velocity_initial.Normalize3();
	particle.size = particles_size;
	particle.position = particle.position_initial;

	particle.color = initial_color;
	particle.life = particles_life_time * 1000;
	particle.time_counter = particle.life;
	particle.time_passed = 0.0F;

	particle.geometric_space = float4x4::FromTRS(owner->transform.GetGlobalTranslation(), owner->transform.GetGlobalRotation(), float3::one);
	
	if (orbit)
	{
		particle.orbit_random = rand();
		particle.position.z += sin(particle.orbit_random);
		particle.position.x += cos(particle.orbit_random);
	}
	
	particle.random_velocity_percentage = (float)rand() / RAND_MAX;
	particle.random_size_percentage = (float)rand() / RAND_MAX;
}

void ComponentParticleSystem::Render()
{
	BROFILER_CATEGORY("Particle Render", Profiler::Color::OrangeRed);

	if (active && billboard->billboard_texture != nullptr && billboard->billboard_texture->initialized && num_of_alive_particles > 0)
	{
		unsigned int variation = GetParticlesSystemVariation();
		shader_program = App->program->UseProgram("Particles", variation);

		billboard->CommonUniforms(shader_program);
		static_assert(sizeof(Particle) % (sizeof(float) * 4) == 0); //Check comment on particle struct
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, (sizeof(Particle))*(num_of_alive_particles), particles.data(), GL_STATIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		billboard->quad->RenderInstanced(particles.size());

		glUseProgram(0);
	}

}

void ComponentParticleSystem::Update()
{
	if (!HasToDrawParticleSystem())
	{
		return;
	}

	if (active && playing)
	{
		if (loop && emitting)
		{
			time_counter += App->time->real_time_delta_time;

			if (time_counter >= (time_between_particles * 1000))
			{
				int spawning_count = time_counter / (time_between_particles * 1000);
				int unused_particle = 0;

				for (int i = 0; i < spawning_count; i++)
				{
					unused_particle = FirstUnusedParticle();
					if (unused_particle != 0 || particles[unused_particle].life <= 0.0f)
					{
						RespawnParticle(particles[unused_particle]);
					}
					time_counter = 0.0F;
				}
			}
		}

		CalculateGravityVector();

		// update all particles
		num_of_alive_particles = 0;
		for (unsigned int i = 0; i < playing_particles_number; i++)
		{
			Particle& p = particles[i];

			if (p.life > 0.0f)
			{
				UpdateParticle(p);
				std::iter_swap(particles.begin() + i, particles.begin() + num_of_alive_particles);
				++num_of_alive_particles;
			}
		}
	}
}

void ComponentParticleSystem::UpdateParticle(Particle& particle)
{
	float time_increased = App->time->real_time_delta_time;
	particle.life -= time_increased; // reduce life
	particle.time_passed += time_increased;

	//velocity over time
	float4 velocity = particle.velocity_initial * velocity_particles_start * velocity_factor_mod;
	float4 vel_curve_interpolated = float4::zero;
	float4 acceleration = gravity_vector;
	if (velocity_over_time)
	{
		switch (type_of_velocity_over_time)
		{
		case TypeOfVelocityOverTime::VEL_CONSTANT:
			velocity *= velocity_over_time_speed_modifier;
			break;
		case TypeOfVelocityOverTime::VEL_LINEAR:
		{
			float vel_perc = (velocity_over_time_speed_modifier_second - velocity_over_time_speed_modifier) * (particle.time_passed / particles_life_time * 0.001f);
			velocity *= vel_perc + velocity_over_time_speed_modifier;
			break;
		}
		case TypeOfVelocityOverTime::VEL_RANDOM_BETWEEN_TWO_CONSTANTS:
			velocity *= velocity_over_time_speed_modifier + (velocity_over_time_speed_modifier_second - velocity_over_time_speed_modifier) * particle.random_velocity_percentage;
			break;
		case TypeOfVelocityOverTime::VEL_CURVE:
			velocity *= velocity_over_time_speed_modifier;
			float curve_value = vel_curve.BezierValue(particle.time_passed / particles_life_time * 0.001f).y;
			float vel_in_range = (velocity_over_time_speed_modifier_second - velocity_over_time_speed_modifier) * curve_value + velocity_over_time_speed_modifier;
			vel_curve_interpolated = particle.velocity_initial * vel_in_range * velocity_factor_mod;
			break;
		}
	}

	//update position
	particle.position += (((velocity + vel_curve_interpolated) * time_increased) + (acceleration * particle.time_passed * particle.time_passed / 2));

	if (orbit)
	{
		particle.position.z += sin((particle.time_passed * 0.001f) + particle.orbit_random) - sin(((particle.time_passed - time_increased) * 0.001f) + particle.orbit_random);
		particle.position.x += cos((particle.time_passed * 0.001f) + particle.orbit_random) - cos(((particle.time_passed - time_increased) * 0.001f) + particle.orbit_random);
	}

	//alpha fade
	if (fade)
	{
		float progress = particle.time_passed * 0.001f / fade_time;
		particle.color.w = math::Lerp(0.f, 1.f, 1 - progress);
	}

	//fade color
	switch (type_of_color_change)
	{
	case TypeOfSizeColorChange::COLOR_NONE:
		break;
	case TypeOfSizeColorChange::COLOR_LINEAR:
	{
		color_fade_time = color_fade_time > particles_life_time ? particles_life_time : color_fade_time;

		float progress = particle.time_passed * 0.001f / color_fade_time;
		progress = progress > 1 ? 1 : progress;
		
		particle.color = float4::Lerp(initial_color, color_to_fade, progress);
		break;
	}
	case TypeOfSizeColorChange::COLOR_CURVE:
		float color_value = color_curve.BezierValue(particle.time_passed * 0.001f / particles_life_time).y;
		particle.color = float4::Lerp(initial_color, color_to_fade, color_value);
		break;
	}

	//size change
	if (size_over_time)
	{
		switch (type_of_size_over_time)
		{

		case TypeOfSizeOverTime::SIZE_LINEAR:
		{
			float progress = particle.time_passed * 0.001f / particles_life_time;
			particle.size = particles_size * ((max_size_of_particle - min_size_of_particle) * progress + min_size_of_particle);
			break;
		}
		case TypeOfSizeOverTime::SIZE_RANDOM_BETWEEN_TWO_CONSTANTS:
			particle.size = particles_size * ((max_size_of_particle - min_size_of_particle) * particle.random_size_percentage + 1);
			break;
		case TypeOfSizeOverTime::SIZE_CURVE:
			float size_value = size_curve.BezierValue(particle.time_passed * 0.001f / particles_life_time).y;
			float size_in_range = (max_size_of_particle - min_size_of_particle) * size_value + min_size_of_particle;
			particle.size = particles_size * size_in_range;
			break;
		}
	}

	//animation
	if (billboard->is_spritesheet && !tile_random)
	{
		float progress = particle.time_passed * 0.001f / particles_life_time;
		billboard->ComputeAnimationFrame(progress);

		particle.current_sprite_x = billboard->current_sprite_x;
		particle.current_sprite_y = billboard->current_sprite_y;
	}

	if (follow_owner)
	{
		particle.geometric_space = float4x4::FromTRS(owner->transform.GetGlobalTranslation(), owner->transform.GetGlobalRotation(), float3::one);
	}
	particle.model = particle.geometric_space * float4x4::FromTRS(particle.position.xyz(), Quat::identity, float3(particle.size, 1.f));
	particle.model = particle.model.Transposed();
}

void ComponentParticleSystem::SetParticleTexture(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;
	billboard->ChangeTexture(texture_uuid);
}
void ComponentParticleSystem::Delete()
{
	App->effects->RemoveComponentParticleSystem(this);
}

unsigned int ComponentParticleSystem::GetParticlesSystemVariation()
{
	unsigned int variation = 0;

	switch (billboard->alignment_type)
	{
	case ComponentBillboard::AlignmentType::VIEW_POINT:
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_BILLBOARD_VIEWPOINT_ALIGNMENT);
		break;

	case ComponentBillboard::AlignmentType::AXIAL:
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_BILLBOARD_AXIAL_ALIGNMENT);
		break;
	}

	if (billboard->is_spritesheet)
	{
		variation |= static_cast<unsigned int>(ModuleProgram::ShaderVariation::ENABLE_SPRITESHEET);
	}

	return variation;
}

void ComponentParticleSystem::SpecializedSave(Config& config) const
{
	billboard->SpecializedSave(config);
	config.AddInt(static_cast<int>(type_of_particle_system), "Type of particle system");

	config.AddBool(loop, "EmissionLoop");
	config.AddBool(active, "Active");
	config.AddFloat2(particles_size, "Particle Size");
	config.AddBool(tile_random, "Tile random");
	config.AddFloat(max_tile_value, "Max Tile");
	config.AddFloat(min_tile_value, "Min Tile");

	config.AddFloat(velocity_particles_start, "Velocity of particles");
	config.AddFloat(gravity_modifier, "Gravity modifier");

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

	config.AddInt(max_particles_number, "Max particles");

	config.AddFloat(inner_radius, "Inner Radius");
	config.AddFloat(outer_radius, "Outer Radius");

	config.AddInt(type_of_color_change, "Type of Color Fade");
	config.AddColor(initial_color, "Intial Color");
	config.AddColor(color_to_fade, "Color to fade");
	config.AddFloat(color_fade_time, "Color Fade Time");
	color_curve.SpecializedSave(config, "Color Curve");

	config.AddBool(fade, "Fade");
	config.AddFloat(fade_time, "Fade Time");
	config.AddBool(orbit, "Orbit");

	config.AddInt(type_of_velocity_over_time, "Type of Velocity");
	config.AddBool(velocity_over_time, "Velocity Random");
	config.AddFloat(velocity_over_time_speed_modifier, "Velocity Speed First");
	config.AddFloat(velocity_over_time_speed_modifier_second, "Velocity Speed Second");
	vel_curve.SpecializedSave(config, "Velocity Curve");

	config.AddBool(size_over_time, "Size Over Time");
	config.AddInt(type_of_size_over_time, "Type of Size");
	config.AddFloat(min_size_of_particle, "Max Size Particles");
	config.AddFloat(max_size_of_particle, "Min Size Particles");
	size_curve.SpecializedSave(config, "Size Curve");
	}

void ComponentParticleSystem::SpecializedLoad(const Config& config)
{
	billboard->SpecializedLoad(config);
	type_of_particle_system = static_cast<TypeOfParticleSystem>(config.GetInt("Type of particle system", static_cast<int>(TypeOfParticleSystem::BOX)));

	loop = config.GetBool("EmissionLoop", true);
	min_size_of_particle = config.GetFloat("Max Size Particles", 0.2);
	max_size_of_particle = config.GetFloat("Min Size Particles", 0.2);
	config.GetFloat2("Particle Size", particles_size, float2(0.2f));
	tile_random = config.GetBool("Tile random", false);
	max_tile_value = config.GetFloat("Max Tile", 0);
	min_tile_value = config.GetFloat("Min Tile", 4);

	velocity_particles_start = config.GetFloat("Velocity of particles", 1.0F);
	gravity_modifier = config.GetFloat("Gravity modifier", 0.f);

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

	max_particles_number = config.GetInt("Max particles", MAX_PARTICLES);
	playing_particles_number = max_particles_number;
	inner_radius = config.GetFloat("Inner Radius", 1.0F);
	outer_radius = config.GetFloat("Outer Radius", 3.0F);

	type_of_color_change = static_cast<TypeOfSizeColorChange>(config.GetInt("Type of Color Fade", TypeOfSizeColorChange::COLOR_NONE));
	config.GetColor("Intial Color", initial_color, float4::one);
	config.GetColor("Color to fade", color_to_fade, float4::one);
	color_fade_time = config.GetFloat("Color Fade Time", 1.0F);
	color_curve.SpecializedLoad(config, "Color Curve");

	fade = config.GetBool("Fade", false);
	fade_time = config.GetFloat("Fade Time", 1.0F);
	orbit = config.GetBool("Orbit", false);

	type_of_velocity_over_time = static_cast<TypeOfVelocityOverTime>(config.GetInt("Type of Velocity", TypeOfVelocityOverTime::VEL_CONSTANT));
	velocity_over_time = config.GetBool("Velocity Random", false);
	velocity_over_time_speed_modifier = config.GetFloat("Velocity Speed First", 0.f);
	velocity_over_time_speed_modifier_second = config.GetFloat("Velocity Speed Second", 0.f);
	vel_curve.SpecializedLoad(config, "Velocity Curve");

	size_over_time = config.GetBool("Size Over Time", false);
	type_of_size_over_time = static_cast<TypeOfSizeOverTime>(config.GetInt("Type of Size", TypeOfSizeOverTime::SIZE_LINEAR));
	min_size_of_particle = config.GetFloat("Max Size Particles", 1.0f);
	max_size_of_particle = config.GetFloat("Min Size Particles", 1.0f);
	size_curve.SpecializedLoad(config, "Size Curve");
}

void ComponentParticleSystem::ReassignResource()
{
	if (billboard)
	{
		billboard->ReassignResource();
	}
}

Component* ComponentParticleSystem::Clone(GameObject* owner, bool original_prefab)
{

	ComponentParticleSystem* created_component;
	if (original_prefab)
	{
		created_component = new ComponentParticleSystem();
	}
	else
	{
		created_component = App->effects->CreateComponentParticleSystem();
	}
	CloneBase(static_cast<Component*>(created_component));

	*created_component = *this;
	created_component->Init();
	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
	assert(billboard->emissive_intensity>-1);
	this->billboard->CopyTo(created_component->billboard);
	created_component->billboard->owner = owner;
	return created_component;
};

void ComponentParticleSystem::CopyTo(Component* component_to_copy) const
{
	*component_to_copy = *this;
	ComponentParticleSystem* component_particle_system = static_cast<ComponentParticleSystem*>(component_to_copy);
	auto original_billboard = component_particle_system->billboard;
	*component_particle_system = *this;
	*original_billboard = *this->billboard;
	component_particle_system->billboard = original_billboard;
}

void ComponentParticleSystem::Emit(size_t count)
{
	playing = true;
	int unused_particle = 0;

	for (int i = 0; i < count; i++)
	{
		unused_particle = FirstUnusedParticle();
		if (unused_particle != 0 || particles[unused_particle].life <= 0.0f)
		{
			RespawnParticle(particles[unused_particle]);
		}
	}
}

void ComponentParticleSystem::Play()
{
	playing = true;
	emitting = true;
	playing_particles_number = MAX_PARTICLES;
}

void ComponentParticleSystem::Stop()
{
	time_counter = 0.0F;

	playing = true;
	emitting = false;
}

ENGINE_API void ComponentParticleSystem::Pause()
{
	playing = false;
	emitting = false;
}

void ComponentParticleSystem::CalculateGravityVector()
{
	float4x4 gravity_rotation = float4x4::FromTRS(float3::one, owner->transform.GetGlobalRotation(), float3::one);
	gravity_rotation.Transpose();
	gravity_vector = gravity_rotation * float4(0, gravity_modifier * gravity_factor_mod, 0, 0);
}

void ComponentParticleSystem::Disable()
{
	active = false;
	Stop();
}

void ComponentParticleSystem::Enable()
{
	active = true;
}

ENGINE_API bool ComponentParticleSystem::IsEmitting() const
{
	return emitting;
}

ENGINE_API bool ComponentParticleSystem::HasParticlesAlive() const
{
	return num_of_alive_particles > 0;
}

ENGINE_API bool ComponentParticleSystem::IsPlaying() const
{
	return playing;
}

bool ComponentParticleSystem::HasToDrawParticleSystem() const
{
	if (App->time->isGameRunning() || App->renderer->game_viewport->effects_draw_all)
	{
		return owner->IsEnabled();
	}

	if (App->editor->selected_game_object == nullptr)
	{
		return false;
	}

	if (owner->IsEnabled())
	{
		return App->editor->selected_game_object->UUID == owner->UUID;
	}

	return false;
}
