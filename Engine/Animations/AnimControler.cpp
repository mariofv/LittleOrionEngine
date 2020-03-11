#include "AnimControler.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"


AnimControler::AnimControler()
{
	anim = App->resources->Load<Animation>("Library/Animations/2719853001_Take 001.anim").get();
	animation_time = anim->duration;
}


AnimControler::~AnimControler()
{
}

void AnimControler::Play()
{ 
	if (!stop) 
	{
		Update();
		Play();
	}
}

void AnimControler::Stop()
{
	loop = false;
	stop = true;
}

void AnimControler::Update()
{
	if (loop) {
		current_time = current_time + App->time->delta_time;
		
		progress_animation = (current_time%animation_time / animation_time);
	}
	else
		current_time = 0.f;
}

void AnimControler::GetTransform(char * name, float3 & pos, Quat & rot)
{
	float current_sample;
	//CurrentSample = (current_time*(NumSamples - 1)) / animation_time;
	float3 next_pos;
	Quat next_rot;
	pos = Interpolate(pos, next_pos, 0.3);
	rot = Interpolate(rot, next_rot, 0.3);

	
}

const float3 AnimControler::Interpolate(const float3& first, const float3& second, float lambda)
{
	return first * (1.0f - lambda) + second * lambda;
}

const Quat AnimControler::Interpolate(const Quat& first, const Quat& second, float lambda)
{
	Quat result;
	float dot = first.Dot(second);
	if (dot >= 0.0f) // Interpolate through the shortest path
	{
		result.x = first.x*(1.0f - lambda) + second.x*lambda;
		result.y = first.y*(1.0f - lambda) + second.y*lambda;
		result.z = first.z*(1.0f - lambda) + second.z*lambda;
		result.w = first.w*(1.0f - lambda) + second.w*lambda;
	}
	else
	{
		result.x = first.x*(1.0f - lambda) - second.x*lambda;
		result.y = first.y*(1.0f - lambda) - second.y*lambda;
		result.z = first.z*(1.0f - lambda) - second.z*lambda;
		result.w = first.w*(1.0f - lambda) - second.w*lambda;
	}
	result.Normalize();
	return result;
}
