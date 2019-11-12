#include "Globals.h"
#include "Application.h"
#include "ModuleTime.h"
#include "Timer.h"

ModuleTime::~ModuleTime()
{
	delete real_time_clock;
	delete game_time_clock;
}

// Called before render is available
bool ModuleTime::Init()
{
	LOG("Initializing Engine clocks");
	real_time_clock = new Timer();
	game_time_clock = new Timer();

	real_time_clock->Start();
	game_time_clock->Start();

	return true;
}

update_status ModuleTime::PreUpdate()
{
	++frame_count;
	frame_start_time = game_time_clock->Read();
	real_frame_start_time = real_time_clock->Read();
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleTime::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleTime::PostUpdate()
{
	delta_time = game_time_clock->Read() - frame_start_time;
	real_time_delta_time = real_time_clock->Read() - real_frame_start_time;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleTime::CleanUp()
{
	delete real_time_clock;
	delete game_time_clock;

	return true;
}


float ModuleTime::RealTimeSinceStartup()
{
	return real_time_clock->Read();
}

float ModuleTime::Time()
{
	return game_time_clock->Read();
}

