#include "Timer.h"

#include <SDL/SDL.h>

Timer::~Timer()
{
}

void Timer::Start()
{
	start_ticks = SDL_GetTicks();
	running = true;
}

bool Timer::Started()
{
	return running;
}

float Timer::Read() const
{
	float current_time;

	if (running)
	{
		if (paused) 
		{
			current_time = (pause_start_ticks - start_ticks) - elapsed_while_paused_ticks;
		}
		else
		{
			current_time = (SDL_GetTicks() - start_ticks) - elapsed_while_paused_ticks;
		}
		
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
		elapsed_while_paused_ticks += SDL_GetTicks() - pause_start_ticks;
		paused = false;
	}
}

float Timer::Pause()
{
	if (!paused)
	{
		pause_start_ticks = SDL_GetTicks();
		paused = true;
	}	
	return pause_start_ticks - start_ticks - elapsed_while_paused_ticks;
}

bool Timer::IsPaused() const
{
	return paused;
}

float Timer::Stop()
{
	end_time = SDL_GetTicks() - start_ticks - elapsed_while_paused_ticks;
	running = false;
	paused = false;

	return end_time;
}