#include "TimerUs.h"

const unsigned int TimerUs::frequency = SDL_GetPerformanceFrequency();

void TimerUs::Start()
{
	start_time = ((float)SDL_GetPerformanceCounter())/ TimerUs::frequency;
	running = true;
}

float TimerUs::Read() const
{
	float current_time = running ? ((float)SDL_GetPerformanceCounter()) / TimerUs::frequency - start_time : end_time;
	return current_time;
}

void TimerUs::Stop()
{
	end_time = ((float)SDL_GetPerformanceCounter()) / TimerUs::frequency - start_time;
	running = false;
}
