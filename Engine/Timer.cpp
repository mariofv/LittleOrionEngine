#include "Timer.h"

#include "SDL.h"

Timer::~Timer()
{
}

void Timer::Start()
{
	start_time = SDL_GetTicks();
	running = true;
}

const float Timer::Read() const
{
	float current_time = running ? SDL_GetTicks() - start_time : end_time;
	return current_time;
}

void Timer::Stop()
{
	end_time = SDL_GetTicks() - start_time;
	running = false;
}