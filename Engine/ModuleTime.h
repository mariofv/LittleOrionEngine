#pragma once
#include "Module.h"
#include "Globals.h"

class Timer;

class ModuleTime : public Module
{
public:
	ModuleTime() = default;
	~ModuleTime();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	float RealTimeSinceStartup();
	float Time();

public:
	long frame_count = 0;

	float time_scale = 1.f;
	float delta_time = 0.f;
	
	float real_time_delta_time = 0.f;

private:
	Timer *real_time_clock = nullptr;
	Timer *game_time_clock = nullptr;

	float frame_start_time = 0.f;
	float real_frame_start_time = 0.f;
};
