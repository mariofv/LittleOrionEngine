#include "Tween.h"

void Tween::Update(float dt)
{
	if (state == TweenState::COMPLETED || state == TweenState::PAUSED) return;

	current_time += dt;

	//Do the tweening ops

	if (current_time >= duration) state == TweenState::COMPLETED;
}
