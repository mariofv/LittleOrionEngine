
#ifndef _TWEENSEQUENCE_H_
#define _TWEENSEQUENCE_H_

#include "Animation/Tween/Tween.h"

#include <vector>

class TweenSequence
{
public:
	enum TweenSequenceState
	{
		PLAYING, PAUSED, STOPPED, DISABLED
	};

	TweenSequence* Append(Tween*);
	TweenSequence* Join(Tween*);
	TweenSequence* Insert(float, Tween*);

	TweenSequence* Play();
	TweenSequence* Stop();
	TweenSequence* Pause();
private:
	void Update(float);

	float current_time = 0.0f;
	int last_tween_index = 0;
	TweenSequenceState state = TweenSequenceState::DISABLED;
	std::vector<Tween*> tweens;
	std::vector<Tween*> current_played_tweens;

	friend class LOTween;
};
#endif // !_TWEENSEQUENCE_H_

