#ifndef _TWEENSEQUENCE_H_
#define _TWEENSEQUENCE_H_
#define ENGINE_EXPORTS

#include "Animation/Tween/Tween.h"
#include <vector>

class TweenSequence
{
public:
	enum TweenSequenceState
	{
		PLAYING, PAUSED, STOPPED, DISABLED
	};

	ENGINE_API TweenSequence* Append(Tween*);
	ENGINE_API TweenSequence* Join(Tween*);
	ENGINE_API TweenSequence* Insert(float, Tween*);

	ENGINE_API TweenSequence* Play();
	ENGINE_API TweenSequence* Stop();
	ENGINE_API TweenSequence* Pause();
private:
	void Update(float);

	float current_time = 0.0f;
	TweenSequenceState state = TweenSequenceState::DISABLED;
	std::vector<Tween*> tweens;
	std::vector<Tween*> current_played_tweens;

	friend class LOTween;
};
#endif _TWEENSEQUENCE_H_

