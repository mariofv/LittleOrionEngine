#ifndef _TIMERUS_H_
#define _TIMERUS_H_

#include "Timer.h"

#include <SDL/SDL.h>

class TimerUs
{
public:
	TimerUs() = default;
	~TimerUs() = default;

	void Start();
	float Read() const;
	void Stop();

public:
	float start_time = 0;
	float end_time = 0;

	bool running = false;

	static const unsigned int frequency;
};

#endif // _TIMERUS_H_
