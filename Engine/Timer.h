#ifndef _TIMER_H_
#define _TIMER_H_

class Timer
{
public:
	Timer() = default;
	~Timer();

	void Start();
	float Stop();

	void Resume();
	float Pause();
	bool IsPaused() const;

	float Read() const;

private:
	float start_ticks = 0;
	float end_time = 0;

	float pause_start_ticks = 0;
	float elapsed_while_paused_ticks = 0;

	bool running = false;
	bool paused = false;
};

#endif // _TIMER_H_

