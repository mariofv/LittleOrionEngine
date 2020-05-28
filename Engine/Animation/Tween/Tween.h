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
		LINEAR
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

	void Update(float);

	friend class TweenSequence;
};
#endif // !_TWEEN_H_
