#include "Tween.h"

void Tween::Play()
{
	if (state == TweenState::COMPLETED) return;

	state = TweenState::PLAYING;
}

void Tween::Pause()
{
	if (state != TweenState::PLAYING) return;

	state = TweenState::PAUSED;
}

void Tween::Update(float dt)
{
	if (state == TweenState::COMPLETED || state == TweenState::PAUSED) return;

	current_time += dt;

	//Do the tweening ops

	if (current_time >= duration) state == TweenState::COMPLETED;
}
