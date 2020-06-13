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

	current_time = start_time + dt;

	if (current_time >= duration) state == TweenState::COMPLETED;

	return (end_value - initial_value) * EasedTime();
}

float Tween::NormalizedElapsedTime()
{
	//A = start_time / 0
	//B = start_time + duration / 1

	//C = current_time / y

	return (current_time - start_time) * (1.0f / duration);
}

float Tween::EasedTime()
{
	float t = NormalizedElapsedTime();

	switch (ease_type)
	{
	case EaseType::SMOOTH_STEP:
		t = SmoothStep(t);
		break;

	case EaseType::EASE_IN_SINE:
		t = EaseInSine(t);
		break;

	case EaseType::SINE:
		t = Sine(t);
		break;

	case EaseType::EASE_OUT_SINE:
		t = EaseOutSine(t);
		break;

	case EaseType::EASE_IN_OUT_SINE:
		t = EaseInOutSine(t);
		break;

	case EaseType::EASE_IN_BACK:
		t = EaseInBack(t);
		break;

	case EaseType::EASE_OUT_BACK:
		t = EaseOutBack(t);
		break;

	case EaseType::EASE_IN_OUT_BACK:
		t = EaseInOutBack(t);
		break;

	default:
		t = Linear(t);
		break;
	}

	return t;
}

float Tween::Linear(float t) {
	return t;
}

float Tween::SmoothStep(float t) {
	return (t*t *(3 - 2 * t));
}

float Tween::EaseInSine(float t) {
	return 1 - cos(t * (3.1416 / 2));
}

float Tween::Sine(float t) {
	return (sin(t * (3.1416 * 2) - 3.1416 / 2) + 1.0) / 2.0;
}

float Tween::EaseOutSine(float t) {
	return sin(t * (3.1416 / 2));
}

float Tween::EaseInOutSine(float t) {
	return -0.5 * (cos(3.1416 * t) - 1);
}

float Tween::EaseInBack(float t) {
	float s = 1.70158f;
	return t * t*((s + 1)*t - s);
}

float Tween::EaseOutBack(float t) {
	float s = 1.70158f;
	t--;
	return (t*t*((s + 1)*t + s) + 1);
}

float Tween::EaseInOutBack(float t) {
	float s = 1.70158f * 1.525f;
	//float s2 = s * 1.525f;
	t *= 2;
	if (t < 1) return 1.0 / 2 * (t*t*((s + 1)*t - s));
	float postFix = t -= 2;
	return 1.0 / 2 * ((postFix)*t*((s + 1)*t + s) + 2);
}
