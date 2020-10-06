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
}


void Timeline::TranslationTrack::Update(float current_time)
{
	if (next_keyframe == nullptr || next_keyframe->time >= current_time)
	{
		next_keyframe = static_cast<Timeline::TranslationKeyframe*>(GetNextKeyFrame(current_time));
	}

	if (next_keyframe)
	{
		target->transform.GetGlobalTranslation();
		float3 position = float3::Lerp(target->transform.GetGlobalTranslation(), next_keyframe->target_translation, current_time/ next_keyframe->time);
		target->transform.SetGlobalMatrixTranslation(position);
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

void Timeline::Track::AddKeyframe(Timeline::Keyframe keyframe)
{
	keyframes.push_back(keyframe);
}
