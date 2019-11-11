#ifndef _TIMER_H_
#define _TIMER_H_

class Timer
{
public:
	Timer() = default;
	~Timer();

	void Start();
	const float Read() const;
	void Stop();

private:
	float start_time = 0;
	float end_time = 0;

	bool running = false;
};

#endif // _TIMER_H_

