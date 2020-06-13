#include "Tween.h"
#include <math.h>

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

float Tween::Update(float dt)
{
	if (state == TweenState::COMPLETED || state == TweenState::PAUSED) return;

	float current_value = 0.0f;
	current_time = start_time + dt;

	switch (ease_type)
	{
		case EaseType::LINEAR:
			current_value = UpdateLinear();
			break;
	}

	if (current_time >= duration) state == TweenState::COMPLETED;

	return current_value;
}

float Tween::NormalizedElapsedTime()
{
	//A = start_time / 0
	//B = start_time + duration / 1

	//C = current_time / y

	return (current_time - start_time) * (1.0f / duration);

}

float Tween::UpdateLinear()
{
	return (end_value - initial_value) * NormalizedElapsedTime();
}
