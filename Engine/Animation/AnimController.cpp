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
	loop = active_state->clip->loop;
}

void AnimController::GetPose(float current_time,uint32_t skeleton_uuid, std::vector<float4x4> & pose) const
{
	float current_keyframe = (current_time*(animation->frames - 1)) / animation_time;
	size_t first_keyframe_index = static_cast<size_t>(std::floor(current_keyframe));
	size_t second_keyframe_index = static_cast<size_t>(std::ceil(current_keyframe));

	float interpolation_lambda = current_keyframe - std::floor(current_keyframe);


	const std::vector<Animation::Channel> current_pose = animation->keyframes[first_keyframe_index].channels;
	const std::vector<Animation::Channel> next_pose = animation->keyframes[second_keyframe_index].channels;

	if (active_state->clip->skeleton_channels_joints_map.find(skeleton_uuid) == active_state->clip->skeleton_channels_joints_map.end())
	{
		return;
	}
	auto & joint_channels_map = active_state->clip->skeleton_channels_joints_map[skeleton_uuid];
	for (size_t i = 0 ; i < joint_channels_map.size(); ++i)
	{
		size_t joint_index = joint_channels_map[i];
		if (joint_index < pose.size())
		{
			float3 last_translation = current_pose[i].translation;
			float3 next_translation = next_pose[i].translation;

			Quat last_rotation = current_pose[i].rotation;
			Quat next_rotation = next_pose[i].rotation;

			float3 position = Utils::Interpolate(last_translation, next_translation, interpolation_lambda);
			Quat rotation = Utils::Interpolate(last_rotation, next_rotation, interpolation_lambda);
			pose[joint_index] = float4x4::FromTRS(position,rotation,float3::one);
		}
	}
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

std::shared_ptr<State> AnimController::GetNextState(const std::string & trigger) const
{
	std::shared_ptr<Transition> transition = state_machine->GetTransition(trigger, active_state->name_hash);
	std::shared_ptr<State> next_state;
	if (transition != nullptr)
	{
		next_state = state_machine->GetState(transition->target_hash);
	}
	return next_state;
}

void AnimController::SetActiveState(std::shared_ptr<State>& new_state)
{
	active_state = new_state;
	animation = active_state->clip->animation;
	Init();
}
