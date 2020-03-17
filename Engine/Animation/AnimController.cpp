#include "AnimController.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"
#include "Helper/Utils.h"

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

bool AnimController::GetTransform(const std::string& channel_name, float3 & pos, Quat & rot)
{
	float current_sample = (current_time*(anim->duration - 1)) / animation_time;
	int current_keyframe = math::FloorInt(current_sample);

	int next_keyframe = (current_keyframe + 1)%(int)anim->duration;

	float3 current_translation;
	float3 next_translation;

	Quat current_rotation;
	Quat next_rotation;

	bool channel_found = false;
	size_t i = 0;
	while (!channel_found && i < anim->keyframes[current_keyframe].channels.size())
	{
		if (anim->keyframes[current_keyframe].channels[i].name == channel_name)
		{
			channel_found = true;

			current_translation = anim->keyframes[current_keyframe].channels[i].translation;
			next_translation = anim->keyframes[next_keyframe].channels[i].translation;

			current_rotation = anim->keyframes[current_keyframe].channels[i].rotation;
			next_rotation = anim->keyframes[next_keyframe].channels[i].rotation;
		}
		++i;
	}

	if (channel_found)
	{
		float delta = current_sample - current_keyframe;
		pos = Utils::Interpolate(current_translation, next_translation, delta);
		rot = Utils::Interpolate(current_rotation, next_rotation, delta);
	}

	return channel_found;
}