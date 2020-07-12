#ifndef _MODULETIME_H_
#define _MODULETIME_H_

#define ENGINE_EXPORTS

#include "Module.h"
#include "Main/Globals.h"

#include "EditorUI/Panel/PanelToolBar.h"
#include "EditorUI/Panel/PanelConfiguration.h"

class Timer;

class ModuleTime : public Module
{
public:
	ModuleTime() = default;
	~ModuleTime();

	bool Init() override;
	bool CleanUp() override;

	ENGINE_API float GetFPS() const;
	void SetMaxFPS(int fps);
	void SetTimeScale(float time_scale);
	void ResetInitFrame();

	ENGINE_API bool isGameRunning();

	void Play();
	void Pause();
	void StepFrame();

	void EndFrame();

public:
	long frame_count = 0;

	float time_scale = 1.0f;

	float delta_time = 0.f;
	float real_time_delta_time = 0.f;

	float time = 0.f;
	float real_time_since_startup = 0.f;

private:
	Timer *real_time_clock = nullptr;
	Timer *game_time_clock = nullptr;

	float frame_start_time = 0.f;
	float real_frame_start_time = 0.f;

	bool limit_fps = true;
	float last_frame_delay = 0.f;

	int max_fps = 60;
	float current_fps = 0.f;

	bool stepping_frame = false;

	friend class PanelConfiguration;
	friend class PanelToolBar;
};

#endif