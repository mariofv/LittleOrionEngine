#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#include <vector>
#include "MathGeoLib.h"
class GameObject;
class Timeline
{
public:

	struct Keyframe
	{
		float time = 0;
	};

	struct Track
	{
		GameObject* target;
		Keyframe* GetNextKeyFrame(float current_time);
		void AddKeyframe(Keyframe keyframe);
	private:
		std::vector<Keyframe> keyframes;
	};

	struct TranslationKeyframe : Keyframe
	{
		math::float3 target_translation;
		const GameObject* target_translation_optional = nullptr;
	};
	struct TranslationTrack: Track
	{
		void Update(float current_time);
		TranslationKeyframe* next_keyframe = nullptr;
	};

	Timeline(float duration);
	~Timeline() = default;

	void Update();
private:
	float duration;
	std::vector<Track> tracks;
	bool playing = false;
	float time_passed = 0;
};

#endif //_TIMELINE_H_