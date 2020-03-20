#include "AnimController.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"
#include "Helper/Utils.h"

#include <math.h>
#include <algorithm>

AnimController::AnimController()
{
	anim = App->resources->Load<Animation>("Library/Metadata/38/3800295065");
	sk = App->resources->Load<Skeleton>("Library/Metadata/29/2987806620");
	Init();
}

void AnimController::Init()
{
	for (size_t i = 0; i < anim->keyframes[0].channels.size(); ++i)
	{
		auto & channel = anim->keyframes[0].channels[i];
		auto it = std::find_if(sk->skeleton.begin(), sk->skeleton.end(), [channel](const Skeleton::Joint & joint)
		{
			return joint.name == channel.name;
		});
		if (it != sk->skeleton.end())
		{
			Skeleton::Joint joint = (*it);
			while (joint.parent_index < sk->skeleton.size())
			{
				joint = sk->skeleton[joint.parent_index];
				auto it_channel = std::find_if(anim->keyframes[0].channels.begin(), anim->keyframes[0].channels.end(), [&joint](const Animation::Channel & parent_channel)
				{
					return joint.name == parent_channel.name;
				});
				if (it_channel != anim->keyframes[0].channels.end())
				{
					channel_hierarchy_cache[i].push_back(it_channel - anim->keyframes[0].channels.begin());
				}
			}
		}

	}
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

bool AnimController::GetTranslation(const std::string& channel_name, float3 & pos)
{
	float current_sample = (current_time*(anim->frames - 1)) / animation_time;
	int current_keyframe = math::FloorInt(current_sample);

	int next_keyframe = (current_keyframe + 1) % (int)anim->frames;

	float3 current_translation;
	float3 next_translation;

	bool channel_found = false;
	size_t i = 0;
	while (!channel_found && i < anim->keyframes[current_keyframe].channels.size())
	{
		if (anim->keyframes[current_keyframe].channels[i].name == channel_name)
		{
			channel_found = true;
			if (!anim->keyframes[current_keyframe].channels[i].is_translated)
			{
				return false;
			}
			current_translation = anim->keyframes[current_keyframe].channels[i].translation;
			next_translation = anim->keyframes[next_keyframe].channels[i].translation;
		}
		++i;
	}

	if (channel_found)
	{
		float delta = current_sample - current_keyframe;
		pos = Utils::Interpolate(current_translation, next_translation, delta);
	}

	return channel_found;
}

bool AnimController::GetRotation(const std::string& channel_name, Quat & rot)
{
	float current_sample = (current_time*(anim->frames - 1)) / animation_time;
	int current_keyframe = math::FloorInt(current_sample);

	int next_keyframe = (current_keyframe + 1) % (int)anim->frames;

	Quat current_rotation;
	Quat next_rotation;

	bool channel_found = false;
	size_t i = 0;
	while (!channel_found && i < anim->keyframes[current_keyframe].channels.size())
	{
		if (anim->keyframes[current_keyframe].channels[i].name == channel_name)
		{
			channel_found = true;
			if (!anim->keyframes[current_keyframe].channels[i].is_rotated)
			{
				return false;
			}

			current_rotation = anim->keyframes[current_keyframe].channels[i].rotation;
			next_rotation = anim->keyframes[next_keyframe].channels[i].rotation;
		}
		++i;
	}

	if (channel_found)
	{
		float delta = current_sample - current_keyframe;
		rot = Utils::Interpolate(current_rotation, next_rotation, delta);
	}

	return channel_found;
}

bool AnimController::GetTransformation(const std::string& channel_name, float4x4 & transform)
{
	float current_sample = (current_time*(anim->frames - 1)) / animation_time;
	int current_keyframe = math::FloorInt(current_sample);

	int next_keyframe = (current_keyframe + 1) % (int)anim->frames;

	float4x4 current_global_tranform = float4x4::identity;
	float4x4 next_global_tranform = float4x4::identity;

	std::vector<Animation::Channel> & channels = anim->keyframes[current_keyframe].channels;

	bool channel_found = false;
	size_t i = 0;
	while (!channel_found && i < anim->keyframes[current_keyframe].channels.size())
	{
		if (anim->keyframes[current_keyframe].channels[i].name == channel_name)
		{
			channel_found = true;
			GetChannelGlobalTransform(i,anim->keyframes[current_keyframe].channels[i], channels, current_global_tranform);
			GetChannelGlobalTransform(i,anim->keyframes[next_keyframe].channels[i], channels, next_global_tranform);
		}
		++i;
	}

	if (channel_found)
	{
		float delta = current_sample - current_keyframe;
		transform = Utils::Interpolate(current_global_tranform, next_global_tranform, delta);
	}
	return channel_found;
}

void AnimController::GetChannelGlobalTransform(size_t channel_index,const Animation::Channel & channel, const std::vector<Animation::Channel> & channels, math::float4x4 &current_global_tranform)
{
	float4x4 channel_local_tranform = float4x4::FromTRS(channel.translation, channel.rotation, float3(1.0f, 1.0f, 1.0f));
	for (int j = channel_hierarchy_cache[channel_index].size() - 1; j >= 0; --j)
	{
		auto & channel_parent = channels[channel_hierarchy_cache[channel_index][j]];
		current_global_tranform = current_global_tranform * float4x4::FromTRS(channel_parent.translation, channel_parent.rotation, float3(1.0f, 1.0f, 1.0f));
	}
	current_global_tranform = current_global_tranform * channel_local_tranform;
}
