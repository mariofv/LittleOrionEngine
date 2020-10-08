#ifndef _TIMELINE_H_
#define _TIMELINE_H_

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include <vector>
#include <MathGeoLib.h>

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
		Track(GameObject* target) : m_target(target) {};
		virtual void Update(float current_time) = 0;

	protected:
		void AddKeyframe(Keyframe& keyframe);
		Keyframe* GetNextKeyFrame(float current_time);
		GameObject* m_target;
		std::vector<Keyframe> keyframes;
	};

	struct TranslationTrack: Track
	{
		struct TranslationKeyframe : Keyframe
		{
			float4x4 target_transform;
		};
		TranslationTrack(GameObject* target) : Track(target) {};
		void AddKeyframe(float time,const float4x4& transform);
	protected:
		void Update(float current_time) override;
		TranslationKeyframe* next_keyframe = nullptr;
	};

	Timeline(float duration);
	~Timeline() = default;

	void Update();
private:
	float duration;
	std::vector<Track> tracks;
	bool playing = false;
	float current_time = 0;
};

#endif //_TIMELINE_H_