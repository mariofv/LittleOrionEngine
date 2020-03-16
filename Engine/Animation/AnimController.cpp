#include "AnimController.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"

#include <math.h>


AnimController::AnimController()
{
	anim = App->resources->Load<Animation>("Library/Animations/RootNode_Take 001.anim").get();
}


AnimController::~AnimController()
{
}

void AnimController::Play()
{ 
	current_time = 0.f;
	playing = true;
}

void AnimController::Stop()
{
	playing = false;
}

void AnimController::Update()
{
	if (!playing)
	{
		return;
	}

	current_time = current_time + App->time->delta_time;
	if (current_time >= animation_time)
	{
		if (loop) 
		{

			current_time = current_time % animation_time;
		}
		else
		{
			playing = false;
		}
	}

}

void AnimController::GetTransform(char * name, float3 & pos, Quat & rot)
{
	float current_sample = (current_time*(anim->duration - 1)) / animation_time;
	int current_keyframe = math::FloorInt(current_sample);

	int next_keyframe = (current_keyframe + 1)%(int)anim->duration;

	float4x4 current_transformation_matrix;
	float4x4 next_transformation_matrix;

	for (unsigned int i = 0; i < anim->keyframes[current_keyframe].channels.size(); ++i)
	{
		if (anim->keyframes[current_keyframe].channels[i].name == name) {
			current_transformation_matrix = anim->keyframes[current_keyframe].channels[i].position;
			next_transformation_matrix = anim->keyframes[next_keyframe].channels[i].position;
		}
	}

	pos = Interpolate(current_transformation_matrix.TranslatePart(), next_transformation_matrix.TranslatePart(), current_sample - current_keyframe);
	rot = Interpolate(current_transformation_matrix.RotatePart().ToQuat(), next_transformation_matrix.RotatePart().ToQuat(), current_sample - current_keyframe);

	
}

const float3 AnimController::Interpolate(const float3& first, const float3& second, float lambda)
{
	return first * (1.0f - lambda) + second * lambda;
}

const Quat AnimController::Interpolate(const Quat& first, const Quat& second, float lambda)
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
