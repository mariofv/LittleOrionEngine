#include "Tween.h"
#include <math.h>

void Tween::Play()
{
	if (state == TweenState::COMPLETED) return;

	current_time = start_time;
	state = TweenState::PLAYING;
}

void Tween::Pause()
{
	if (state != TweenState::PLAYING) return;

	state = TweenState::PAUSED;
}

Tween * Tween::LOTranslate(ComponentTransform2D* transform, float2 end_value, float desired_time)
{
	Tween* tween = new Tween();
	tween->transform = transform;
	tween->initial_vector = transform->anchored_position;
	tween->desired_vector = end_value;
	tween->duration = desired_time;
	tween->tween_type = TweenType::TRANSLATE;

	return tween;
}

Tween * Tween::LORotate(ComponentTransform2D* transform, float end_value, float desired_time)
{
	Tween* tween = new Tween();
	tween->transform = transform;
	tween->initial_value = transform->GetGlobalRotation().z;
	tween->desired_value = end_value;
	tween->duration = desired_time;
	tween->tween_type = TweenType::ROTATE;

	return tween;
}

Tween * Tween::LOScale(ComponentTransform2D* transform, float end_scale, float desired_time)
{
	Tween* tween = new Tween();
	tween->transform = transform;
	tween->initial_value = 1.0f;
	tween->desired_value = end_scale;
	tween->duration = desired_time;
	tween->tween_type = TweenType::SCALE;

	return tween;
}

Tween * Tween::LOColor(ComponentImage* image, float4 end_color, float desired_time)
{
	Tween* tween = new Tween();
	tween->image = image;
	tween->initial_color = image->color;
	tween->desired_color = end_color;
	tween->duration = desired_time;
	tween->tween_type = TweenType::COLOR;

	return tween;
}

Tween * Tween::SetEase(EaseType ease)
{
	ease_type = ease;

	return this;
}

void Tween::Update(float dt)
{
	if (state == TweenState::COMPLETED || state == TweenState::PAUSED) return;

	current_time += dt / 1000.0f;

	float progress = UpdateTweenByType();

	if (progress >= 1.0f) state == TweenState::COMPLETED;
}

float Tween::UpdateTweenByType()
{
	if (tween_type == Tween::NONE) return 0.0f;

	float eased_time = EasedTime();

	switch (tween_type)
	{
	case Tween::TRANSLATE:
		tweened_vector = float2::Lerp(initial_vector, desired_vector, eased_time);
		transform->SetTranslation(float3(tweened_vector, 0));
		break;

	case Tween::ROTATE:
		tweened_value = math::Lerp(initial_value, desired_value, eased_time);
		transform->SetRotation(float3(0, 0, tweened_value));
		break;

	case Tween::SCALE:
		tweened_value = math::Lerp(initial_value, desired_value, eased_time);
		transform->SetScale(float3::one * tweened_value);
		break;

	case Tween::COLOR:
		tweened_color = float4::Lerp(initial_color, desired_color, eased_time);
		image->SetColor(tweened_color);
		break;
	}

	return eased_time;
}

float Tween::NormalizedElapsedTime()
{
	//A = start_time / 0
	//B = start_time + duration / 1

	//C = current_time / y

	float normalized_time =  (current_time - start_time) * (1.0f / duration);
	APP_LOG_INFO("Tween: Start Time = %f, Current Time = %f, Duration = %f, Normailized Time = %f", start_time, current_time, duration, normalized_time);

	return normalized_time;
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
