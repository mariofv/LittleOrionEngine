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

float Timer::Read() const
{
	float current_time;
	if (running)
	{
		current_time = (paused ? pause_start_time : SDL_GetTicks()) - start_time - pause_time;
	} 
	else
	{
		current_time = end_time;
	} 
	return current_time;
}

void Timer::Resume()
{
	if (paused)
	{
		pause_time += (SDL_GetTicks() - pause_start_time);
		paused = false;
	}
}

float Timer::Pause()
{
	if (!paused)
	{
		pause_start_time = SDL_GetTicks();
		paused = true;
	}	
	return pause_start_time - start_time - pause_time;
}

float Timer::Stop()
{
	end_time = SDL_GetTicks() - start_time - pause_time;
	running = false;
	return end_time;
}