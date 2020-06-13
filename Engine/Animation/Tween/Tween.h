#ifndef _TWEEN_H_
#define _TWEEN_H_
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

	TweenState state;

	void Play();
	void Pause();
	//tbd later
	//void OnUpdate();
	//void OnComplete();

private:
	float start_time = 0.0f;
	float initial_value = 0.0f;
	float end_value = 0.0f;
	float duration = 0.0f;
	float current_time = 0.0f;
	EaseType ease_type = EaseType::LINEAR;

	float Update(float);

	float NormalizedElapsedTime();
	float EasedTime();

	float Linear(float);
	float SmoothStep(float);
	float EaseInSine(float);
	float Sine(float);
	float EaseOutSine(float);
	float EaseInOutSine(float);
	float EaseInBack(float);
	float EaseOutBack(float);
	float EaseInOutBack(float);

	friend class TweenSequence;
};
#endif _TWEEN_H_
