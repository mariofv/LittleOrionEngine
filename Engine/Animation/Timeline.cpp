#include "Timeline.h"
#include "Module/ModuleTime.h"
#include "Main/Application.h"
#include "Main/GameObject.h"

#include "Component/ComponentTransform.h"
Timeline::Timeline(float duration)
	: duration(duration)
{
}

void Timeline::Update()
{
	current_time += App->time->delta_time;
	if (current_time > duration)
	{
		playing = false;
		return;
	}
	for (auto& track : tracks)
	{
		track.Update(current_time);
	}
}


void Timeline::TranslationTrack::AddKeyframe(float time, const float4x4& transform)
{
	TranslationKeyframe keyframe;
	keyframe.target_transform = transform;
	keyframe.time = time;
	Track::AddKeyframe(keyframe);
}

void Timeline::TranslationTrack::Update(float current_time)
{
	if (next_keyframe == nullptr || next_keyframe->time >= current_time)
	{
		next_keyframe = static_cast<TranslationKeyframe*>(GetNextKeyFrame(current_time));
	}

	if (next_keyframe)
	{
		float4x4 global_matrix = m_target->transform.GetGlobalModelMatrix();
		float3 position = float3::Lerp(global_matrix.TranslatePart(), next_keyframe->target_transform.TranslatePart(), current_time/next_keyframe->time);
		m_target->transform.SetGlobalMatrixTranslation(position);

		Quat rotation = Quat::Lerp(global_matrix.RotatePart().ToQuat(), next_keyframe->target_transform.RotatePart().ToQuat(), current_time / next_keyframe->time);
		m_target->transform.SetGlobalMatrixRotation(rotation);

		float3 scale = float3::Lerp(global_matrix.ExtractScale(), next_keyframe->target_transform.ExtractScale(), current_time / next_keyframe->time);
		m_target->transform.SetGlobalMatrixScale(scale);
	}
}


Timeline::Keyframe* Timeline::Track::GetNextKeyFrame(float current_time)
{
	for (Timeline::Keyframe& keyframe : keyframes)
	{
		if (keyframe.time > current_time)
		{
			return &keyframe;
		}
	}
	return nullptr;
}

void Timeline::Track::AddKeyframe(Timeline::Keyframe& keyframe)
{
	keyframes.push_back(keyframe);
}
