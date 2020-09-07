#ifndef _TWEENSEQUENCE_H_
#define _TWEENSEQUENCE_H_
#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "Animation/Tween/Tween.h"
#include <vector>
#include <functional>

class TweenSequence
{
public:
	enum TweenSequenceState
	{
		PLAYING, PAUSED, STOPPED, DISABLED
	};

	void Clear();

	ENGINE_API TweenSequence* Append(Tween*);
	ENGINE_API TweenSequence* Join(Tween*);
	ENGINE_API TweenSequence* Insert(float, Tween*);

	ENGINE_API TweenSequence* Play();
	ENGINE_API TweenSequence* Stop();
	ENGINE_API TweenSequence* Pause();

	ENGINE_API TweenSequence* OnCompleted(std::function<void(void)> callback);


private:
	void Update(float);

	float current_time = 0.0f;
	TweenSequenceState state = TweenSequenceState::DISABLED;
	std::vector<Tween*> tweens;
	std::vector<Tween*> current_played_tweens;
	std::function<void(void)> on_completed_callback = nullptr;

	friend class LOTween;
};
#endif _TWEENSEQUENCE_H_

