#ifndef _TWEEN_H_
#define _TWEEN_H_
#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif


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

	ENGINE_API static Tween* LOTranslate(ComponentTransform2D* transform, const float2 desired_position, float time);
	ENGINE_API static Tween* LORotate(ComponentTransform2D* transform, const float desired_rotation, float time);
	ENGINE_API static Tween* LOScale(ComponentTransform2D* transform, const float3 desired_scale, float time);
	ENGINE_API static Tween* LOColor(ComponentImage* image, const float4 desired_color, float time);
	ENGINE_API Tween* SetEase(EaseType ease);
	ENGINE_API Tween* SetLoops(const int loops, TweenLoop loop_type);

private:
	float start_time = 0.0f;
	float duration = 0.0f;
	float current_time = 0.0f;
	
	float2 initial_vector = float2::zero;
	float2 tweened_vector = float2::zero;
	float2 desired_vector = float2::zero;
	
	float3 initial_scale = float3::zero;
	float3 tweened_scale = float3::zero;
	float3 desired_scale = float3::zero;

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

	void Update(float delta_time);
	float UpdateTweenByType();
	void CheckFinishTween(float delta_time);

	float NormalizedElapsedTime();
	float EasedTime();

	void ResetTween();

	float Linear(const float time) const;
	float SmoothStep(const float time) const;
	float EaseInSine(const float time) const;
	float Sine(const float time) const;
	float EaseOutSine(const float time) const;
	float EaseInOutSine(const float time) const;
	float EaseInBack(const float time) const;
	float EaseOutBack(const float time) const;
	float EaseInOutBack(const float time) const;

	friend class TweenSequence;
};
#endif _TWEEN_H_
