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

void AnimController::GetPose(uint32_t skeleton_uuid, std::vector<float4x4> & pose) const
{
	for (auto & clip : playing_clips)
	{
		std::vector<float4x4> fading_pose(pose.size());
		GetClipTransform(clip.current_time,skeleton_uuid, clip.clip, fading_pose);
		if (playing_clips.size() == 1)
		{
			pose = fading_pose;
		}
		else
		{
			pose = InterpolatePoses(pose, fading_pose);
		}
	}
	if (active_transition && active_transition->interpolation_time < fade_time)
	{
	}
}

std::vector<float4x4> AnimController::InterpolatePoses(const std::vector<float4x4>& first_pose, const std::vector<float4x4>& second_pose) const
{
	assert(first_pose.size() == second_pose.size());
	std::vector<float4x4> interpolated_pose(first_pose.size());
	for (size_t i = 0; i < first_pose.size(); i++)
	{
		interpolated_pose[i] = Utils::Interpolate(first_pose[i], second_pose[i],0);
	}
	return interpolated_pose;
}

void AnimController::GetClipTransform(float current_time, uint32_t skeleton_uuid, const std::shared_ptr<Clip> &clip, std::vector<math::float4x4> & pose) const
{
	float current_keyframe = (current_time*(clip->animation->frames - 1)) / clip->animation_time;
	size_t first_keyframe_index = static_cast<size_t>(std::floor(current_keyframe));
	size_t second_keyframe_index = static_cast<size_t>(std::ceil(current_keyframe));

	float interpolation_lambda = current_keyframe - std::floor(current_keyframe);

	const std::vector<Animation::Channel> current_pose = clip->animation->keyframes[first_keyframe_index].channels;
	const std::vector<Animation::Channel> next_pose = clip->animation->keyframes[second_keyframe_index].channels;

	if (clip->skeleton_channels_joints_map.find(skeleton_uuid) == clip->skeleton_channels_joints_map.end())
	{
		return;
	}
	auto & joint_channels_map = clip->skeleton_channels_joints_map[skeleton_uuid];
	for (size_t i = 0; i < joint_channels_map.size(); ++i)
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
			pose[joint_index] = float4x4::FromTRS(position, rotation, float3::one);
		}
	}
}

void AnimController::SetActiveAnimation()
{
	active_state = state_machine->GetDefaultState();
	if (active_state != nullptr && active_state->clip != nullptr)
	{
		playing_clips.push_back({ active_state->clip });
	}
}

std::shared_ptr<State> AnimController::StartNextState(const std::string & trigger)
{
	active_transition = state_machine->GetTransition(trigger, active_state->name_hash);
	std::shared_ptr<State> next_state;
	if (active_transition != nullptr)
	{
		next_state = state_machine->GetState(active_transition->target_hash);
	}
	playing_clips.push_back({ next_state->clip });
	return next_state;
}

void AnimController::SetActiveState(std::shared_ptr<State>& new_state)
{
	active_state = new_state;
	playing_clips.push_back({ active_state->clip });
}

bool PlayingClip::Update()
{
	bool playing = true;
	current_time = current_time + static_cast<int>(App->time->delta_time);
	if (current_time >= clip->animation_time)
	{
		if (clip->loop)
		{

			current_time = current_time % clip->animation_time;
		}
		else
		{
			playing = false;
		}
	}
	return playing;
}
