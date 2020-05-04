#ifndef _COMPONENTPARTICLESYSTEM_H
#define _COMPONENTPARTICLESYSTEM_H

#include "Component.h"
#include "MathGeoLib.h"

class GameObject;

class ComponentParticleSystem : public Component
{
public:

	ComponentParticleSystem() = default;
	~ComponentParticleSystem() = default;

	ComponentParticleSystem(GameObject* owner);
	ComponentParticleSystem(const ComponentParticleSystem& component_to_copy) = default;

public:

	struct Initialization 
	{
		float3 position;
		float3 rotation;
		float2 life;
		float2 speed;
		float size;
		float duration;
		float max_particles;
		bool loop;
		float whole_speed;
	};
	struct ColorOverLifetime
	{
		float4 color_over_lifetime;
	};

public:
	Initialization initialization;
	ColorOverLifetime color_over_life;
};

#endif