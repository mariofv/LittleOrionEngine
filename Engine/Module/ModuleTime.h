#ifndef _MODULETIME_H_
#define _MODULETIME_H_

#include "Module.h"
#include "Main/Globals.h"

class Timer;

class ModuleTime : public Module
{
public:
	ModuleTime() = default;
	~ModuleTime();

	bool Init() override;
	update_status PreUpdate() override;
	bool CleanUp() override;

	void SetMaxFPS(int fps);
	void SetTimeScale(float time_scale);

	void Play();
	void Pause();
	void StepFrame();

	void ShowTimeControls();

	void EndFrame();


public:
	long frame_count = 0;

	float time_scale = 1.f;

	float delta_time = 0.f;
	float real_time_delta_time = 0.f;

	float time = 0.f;
	float real_time_since_startup = 0.f;

private:
	Timer *real_time_clock = nullptr;
	Timer *game_time_clock = nullptr;

	float frame_start_time = 0.f;
	float real_frame_start_time = 0.f;

	bool limit_fps = false;
	float last_frame_delay = 0.f;

	int max_fps = 60;

	bool stepping_frame = false;

	friend class PanelConfiguration;
	friend class PanelToolBar;
};

#endif