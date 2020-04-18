#include "AnimController.h"
#include "Component/ComponentMeshRenderer.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"
#include "ResourceManagement/Resources/StateMachine.h"
#include "Helper/Utils.h"

#include <math.h>

void AnimController::GetClipTransform( uint32_t skeleton_uuid, std::vector<math::float4x4>& pose)
{
	for (size_t j = 0; j < playing_clips.size(); j++)
	{
		const std::shared_ptr<Clip> clip = playing_clips[j].clip;
		if (!clip || clip->skeleton_channels_joints_map.find(skeleton_uuid) == clip->skeleton_channels_joints_map.end())
		{
			continue;
		}
		float current_keyframe = ((playing_clips[j].current_time*(clip->animation->frames - 1)) / clip->animation_time) + 1;
		size_t first_keyframe_index = static_cast<size_t>(std::floor(current_keyframe));
		size_t second_keyframe_index = static_cast<size_t>(std::ceil(current_keyframe));

		float interpolation_lambda = current_keyframe - std::floor(current_keyframe);
		const std::vector<Animation::Channel> & current_pose = clip->animation->keyframes[first_keyframe_index].channels;
		const std::vector<Animation::Channel> & next_pose = clip->animation->keyframes[second_keyframe_index].channels;

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
				if (j == ClipType::NEXT)
				{
					float weight = playing_clips[j].current_time / (active_transition->interpolation_time * 1.0f);
					pose[joint_index] = Utils::Interpolate(pose[joint_index], float4x4::FromTRS(position, rotation, float3::one), weight);
					if(weight > 1.0f)
					{
						apply_transition = true;
					}
				}
				else
				{
					pose[joint_index] = float4x4::FromTRS(position, rotation, float3::one);

				}	
			}
		}
	}
}

bool AnimController::Update()
{
	for (auto & playing_clip : playing_clips)
	{
		 playing_clip.Update();
	}

	if (apply_transition && active_transition)
	{
		FinishActiveState();
	}
	return playing_clips[0].playing;
}

void AnimController::SetActiveAnimation()
{
	active_state = state_machine->GetDefaultState();
	if (active_state != nullptr && active_state->clip != nullptr)
	{
		playing_clips[0] = { active_state->clip };
	}
}

void AnimController::StartNextState(const std::string& trigger)
{
	active_transition = state_machine->GetTransition(trigger, active_state->name_hash);
	if (!active_transition)
	{
		return;
	}
	std::shared_ptr<State> next_state;
	if (active_transition)
	{
		next_state = state_machine->GetState(active_transition->target_hash);
		playing_clips[1] ={ next_state->clip };
		playing_clips[1].playing = true;
	}
}

void AnimController::FinishActiveState()
{
	std::shared_ptr<State> next_state = state_machine->GetState(active_transition->target_hash);
	active_state = next_state;
	playing_clips[0] = playing_clips[1];
	playing_clips[1] = {};
	active_transition = nullptr;
	apply_transition = false;
}

void PlayingClip::Update()
{
	if (!playing || !clip)
	{
		return;
	}
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
}
