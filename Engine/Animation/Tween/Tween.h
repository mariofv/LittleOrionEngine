#ifndef _TWEEN_H_
#define _TWEEN_H_
#define ENGINE_EXPORTS


#include "Component/ComponentTransform2D.h"
#include "Component/ComponentImage.h"

class Tween
{
public:
	enum TweenState
	{
		NEW, PLAYING, PAUSED, COMPLETED
	};

	enum EaseType
	{
		LINEAR,
		SMOOTH_STEP,
		EASE_IN_SINE,
		SINE,
		EASE_OUT_SINE,
		EASE_IN_OUT_SINE,
		EASE_IN_BACK,
		EASE_OUT_BACK,
		EASE_IN_OUT_BACK
	};

	enum TweenType
	{
		NONE,
		TRANSLATE,
		ROTATE,
		SCALE,
		COLOR
	};

	enum TweenLoop
	{
		RESTART,
		YOYO
	};

	TweenState state;

	void Play();
	void Pause();
	//tbd later
	//void OnUpdate();
	//void OnComplete();

	ENGINE_API static Tween* LOTranslate(ComponentTransform2D*, float2, float);
	ENGINE_API static Tween* LORotate(ComponentTransform2D*, float, float);
	ENGINE_API static Tween* LOScale(ComponentTransform2D*, float, float);
	ENGINE_API static Tween* LOColor(ComponentImage*, float4, float);
	ENGINE_API Tween* SetEase(EaseType);
	ENGINE_API Tween* SetLoops(int, TweenLoop);

private:
	float start_time = 0.0f;
	float duration = 0.0f;
	float current_time = 0.0f;
	
	float2 initial_vector = float2::zero;
	float2 tweened_vector = float2::zero;
	float2 desired_vector = float2::zero;

	float4 initial_color = float4::zero;
	float4 tweened_color = float4::zero;
	float4 desired_color = float4::zero;
	
	float initial_value = 0.0f;
	float tweened_value = 0.0f;
	float desired_value = 0.0f;

	ComponentTransform2D* transform = nullptr;
	ComponentImage* image = nullptr;

	EaseType ease_type = EaseType::LINEAR;
	TweenType tween_type = TweenType::NONE;

	TweenLoop loop_type = TweenLoop::RESTART;
	int loops = 0;
	int played_times = 0;

	void Update(float);
	float UpdateTweenByType();
	void CheckFinishTween(float);

	float NormalizedElapsedTime();
	float EasedTime();

	void ResetTween();
	void IncrementTween();

	float Linear(float) const;
	float SmoothStep(float) const;
	float EaseInSine(float) const;
	float Sine(float) const;
	float EaseOutSine(float) const;
	float EaseInOutSine(float) const;
	float EaseInBack(float) const;
	float EaseOutBack(float) const;
	float EaseInOutBack(float) const;

	friend class TweenSequence;
};
#endif _TWEEN_H_
