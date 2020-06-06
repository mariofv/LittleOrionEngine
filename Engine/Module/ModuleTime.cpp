#include "ModuleTime.h"

#include "EditorUI/EngineLog.h"
#include "Filesystem/PathAtlas.h"
#include "Helper/Timer.h"
#include "Main/Application.h"
#include "ModuleEditor.h"
#include "ModuleAnimation.h"
#include "ModuleScene.h"
#include "ModuleScriptManager.h"
#include "ModuleWindow.h"

#include <SDL/SDL.h>
#include <Brofiler/Brofiler.h>

ModuleTime::~ModuleTime()
{
	delete real_time_clock;
	delete game_time_clock;
}

// Called before render is available
bool ModuleTime::Init()
{
	APP_LOG_SECTION("************ Module Time Init ************");

	APP_LOG_INIT("Initializing Engine clocks");
	real_time_clock = new Timer();
	game_time_clock = new Timer();

	real_time_clock->Start();

#if GAME
	game_time_clock->Start();
#endif

	APP_LOG_SUCCESS("Engine clocks initialized correctly");

	return true;
}

update_status ModuleTime::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}


void ModuleTime::EndFrame()
{
	BROFILER_CATEGORY("End Frame", Profiler::Color::Lavender);
	++frame_count;

	float real_time = real_time_clock->Read();
	real_time_delta_time = real_time - real_frame_start_time;
	float aux_real_frame_start_time = real_time;

	if (limit_fps)
	{
		float remaining_frame_time = 1000.f / max_fps - real_time_delta_time;
		if (remaining_frame_time > 0)
		{
			SDL_Delay(static_cast<Uint32>(remaining_frame_time));
			last_frame_delay = remaining_frame_time;
		}

		real_time = real_time_clock->Read();
		real_time_delta_time = real_time - real_frame_start_time;
		aux_real_frame_start_time = real_time;

	}
	else
	{
		last_frame_delay = 0.f;
	}
	real_frame_start_time = aux_real_frame_start_time;

	float game_time = game_time_clock->Read();
	delta_time = (game_time - frame_start_time) * time_scale;
	frame_start_time = game_time;

	time += delta_time;
	real_time_since_startup += real_time_delta_time;

	if (frame_count % 10 == 0)
	{
		current_fps = 1000.f / real_time_delta_time;
		App->engine_log->LogFPS(current_fps);
		App->engine_log->LogMS(real_time_delta_time);
	}

	if (stepping_frame)
	{
		stepping_frame = false;
		game_time_clock->Pause();
	}
}

// Called before quitting
bool ModuleTime::CleanUp()
{
	delete real_time_clock;
	delete game_time_clock;

	return true;
}

float ModuleTime::GetFPS() const
{
	return current_fps;
}

void ModuleTime::SetMaxFPS(int fps)
{
	max_fps = fps;
}

void ModuleTime::Play()
{
	if (!game_time_clock->Started())
	{
		App->scene->SaveTmpScene();
		game_time_clock->Start();
		frame_start_time = game_time_clock->Read();
		App->animations->PlayAnimations();
	}
	else
	{
		game_time_clock->Stop();
		App->scene->LoadScene();
	}
}

void ModuleTime::Pause()
{
	if (!game_time_clock->Started())
	{
		Play();
	}

	if (game_time_clock->IsPaused())
	{
		game_time_clock->Resume();
		frame_start_time = game_time_clock->Read();
	}
	else {
		game_time_clock->Pause();
	}
}

void ModuleTime::StepFrame()
{
	if (!game_time_clock->Started())
	{
		Play();
	}

	if (game_time_clock->IsPaused())
	{
		game_time_clock->Resume();
	}
	stepping_frame = true;
}

void ModuleTime::SetTimeScale(float time_scale)
{
	this->time_scale = time_scale;
}

bool ModuleTime::isGameRunning()
{
	return game_time_clock->Started();
}