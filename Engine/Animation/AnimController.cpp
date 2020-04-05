#include "AnimController.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"
#include "Helper/Utils.h"
#include "Component/ComponentMeshRenderer.h"
#include "Main/Application.h"

#include <math.h>
#include <algorithm>
#include "ResourceManagement/Resources/StateMachine.h"

void AnimController::Init()
{
	animation_time = (animation->frames/animation->frames_per_second )* 1000;
}

bool AnimController::GetTransform(float current_time,const std::string & channel_name, float3 & position, Quat & rotation)
{
	float current_keyframe = (current_time*(animation->frames - 1)) / animation_time;
	size_t first_keyframe_index = static_cast<size_t>(std::floor(current_keyframe));
	size_t second_keyframe_index = static_cast<size_t>(std::ceil(current_keyframe));

	float interpolation_lambda = current_keyframe - std::floor(current_keyframe);

	bool channel_found = false;
	size_t channel_index = 0;
	while (!channel_found && channel_index < animation->keyframes[0].channels.size())
	{
		if (animation->keyframes[0].channels[channel_index].name == channel_name)
		{
			channel_found = true;
			break;
		}
		++channel_index;
	}
	if (!channel_found)
		return false;

	float3 last_translation = animation->keyframes[first_keyframe_index].channels[channel_index].translation;
	float3 next_translation = animation->keyframes[second_keyframe_index].channels[channel_index].translation;

	Quat last_rotation = animation->keyframes[first_keyframe_index].channels[channel_index].rotation;
	Quat next_rotation = animation->keyframes[second_keyframe_index].channels[channel_index].rotation;

	position = Utils::Interpolate(last_translation, next_translation, interpolation_lambda);
	rotation = Utils::Interpolate(last_rotation, next_rotation, interpolation_lambda);

	return true;
}

std::shared_ptr<Animation> AnimController::GetCurrentAnimation() const
{
	if (active_state == nullptr || active_state->clip == nullptr)
	{
		return nullptr;
	}
	return active_state->clip->animation;
}

std::vector<float4x4> AnimController::GetPose(float current_time, const std::vector<size_t> & joint_channels_map)
{
	std::vector<float4x4> outpose;
	/*float current_keyframe = (current_time*(animation->frames - 1)) / animation_time;
	size_t first_keyframe_index = static_cast<size_t>(std::floor(current_keyframe));
	size_t second_keyframe_index = static_cast<size_t>(std::ceil(current_keyframe));

	float interpolation_lambda = current_keyframe - std::floor(current_keyframe);


	const std::vector<Animation::Channel> current_pose = animation->keyframes[first_keyframe_index].channels;
	const std::vector<Animation::Channel> next_pose = animation->keyframes[second_keyframe_index].channels;

	for (auto joint : joint_channels_map)
	{
		position = Utils::Interpolate(last_translation, next_translation, interpolation_lambda);
		rotation = Utils::Interpolate(last_rotation, next_rotation, interpolation_lambda);
	}*/
	return outpose;
}


void AnimController::SetActiveAnimation()
{
	active_state = state_machine->GetDefaultState();
	if (active_state != nullptr && active_state->clip != nullptr)
	{
		animation = active_state->clip->animation;
		Init();
	}
}

void AnimController::ActiveAnimation(const std::string & trigger)
{
	std::shared_ptr<Transition> transition = state_machine->GetTransition(trigger, active_state->name_hash);
	std::shared_ptr<State> next_state;
	if (transition != nullptr)
	{
		next_state = state_machine->GetState(transition->target_hash);
	}
	//TODO: BLEND STATES
	active_state = next_state;
	animation = active_state->clip->animation;
	Init();
}
