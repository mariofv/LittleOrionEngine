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

Tween * Tween::LOTranslate(ComponentTransform2D* transform, const float2 end_value, float desired_time)
{
	Tween* tween = new Tween();
	tween->transform = transform;
	tween->initial_vector = transform->anchored_position;
	tween->desired_vector = end_value;
	tween->duration = desired_time;
	tween->tween_type = TweenType::TRANSLATE;

	return tween;
}

Tween * Tween::LORotate(ComponentTransform2D* transform, const float end_value, float desired_time)
{
	Tween* tween = new Tween();
	tween->transform = transform;
	tween->initial_value = transform->GetGlobalRotation().z;
	tween->desired_value = end_value;
	tween->duration = desired_time;
	tween->tween_type = TweenType::ROTATE;

	return tween;
}

Tween * Tween::LOScale(ComponentTransform2D* transform, const float3 end_scale, float desired_time)
{
	Tween* tween = new Tween();
	tween->transform = transform;
	tween->initial_scale = transform->GetGlobalScale();
	tween->desired_scale = end_scale;
	tween->duration = desired_time;
	tween->tween_type = TweenType::SCALE;

	return tween;
}

Tween * Tween::LOColor(ComponentImage* image, const float4 end_color, float desired_time)
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

Tween* Tween::SetLoops(int loops, TweenLoop loop_type)
{
	this->loops = loops;
	this->loop_type = loop_type;

	return this;
}

void Tween::Update(float dt)
{
	if (state == TweenState::COMPLETED || state == TweenState::PAUSED) return;

	current_time += dt / 1000.0f;

	float progress = NormalizedElapsedTime();
	//APP_LOG_INFO("Tween progress: %f", progress);

	UpdateTweenByType();

	CheckFinishTween(progress);
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
		tweened_scale = math::Lerp(initial_scale, desired_scale, eased_time);
		transform->SetScale(tweened_scale);
		break;

	case Tween::COLOR:
		tweened_color = float4::Lerp(initial_color, desired_color, eased_time);
		image->SetColor(tweened_color);
		break;
	}

	return eased_time;
}

void Tween::CheckFinishTween(float progress)
{
	if (progress >= 1.0f)
	{
		played_times += 1;
		current_time = start_time;

		if (loops != -1 && played_times >= loops)
		{
			state = TweenState::COMPLETED;
		}
		else if (loop_type == TweenLoop::RESTART)
		{
			ResetTween();
		}
	}
}

float Tween::NormalizedElapsedTime()
{
	//A = start_time / 0
	//B = start_time + duration / 1

	//C = current_time / y

	return  (current_time - start_time) * (1.0f / duration);
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

	if (loop_type == TweenLoop::YOYO && played_times % 2 == 1) t = 1 - t;

	return t;
}

void Tween::ResetTween()
{
	if (transform != nullptr)
	{
		switch (tween_type)
		{
		case Tween::TRANSLATE:
			transform->SetTranslation(float3(initial_vector, 0.0f));
			break;
		case Tween::ROTATE:
			transform->SetRotation(Quat(0, 0, initial_value, 1));
			break;
		case Tween::SCALE:
			transform->SetScale(float3::one);
			break;
		case Tween::COLOR:
			image->SetColor(initial_color);
			break;
		}
	}
}

float Tween::Linear(float t) const 
{
	return t;
}

float Tween::SmoothStep(float t) const 
{
	return (t*t *(3 - 2 * t));
}

float Tween::EaseInSine(float t) const 
{
	return 1 - cos(t * (math::pi / 2));
}

float Tween::Sine(float t) const 
{
	return (sin(t * (math::pi * 2) - math::pi / 2) + 1.0f) / 2.0f;
}

float Tween::EaseOutSine(float t) const 
{
	return sin(t * (math::pi / 2));
}

float Tween::EaseInOutSine(float t) const 
{
	return -0.5f * (cos(math::pi * t) - 1);
}

float Tween::EaseInBack(float t) const 
{
	float s = 1.70158f;
	return t * t*((s + 1)*t - s);
}

float Tween::EaseOutBack(float t) const 
{
	float s = 1.70158f;
	t--;
	return (t*t*((s + 1)*t + s) + 1);
}

float Tween::EaseInOutBack(float t) const 
{
	float s = 1.70158f * 1.525f;
	//float s2 = s * 1.525f;
	t *= 2;
	if (t < 1) return 1.0f / 2 * (t*t*((s + 1)*t - s));
	float postFix = t -= 2;
	return 1.0f / 2 * ((postFix)*t*((s + 1)*t + s) + 2);
}
