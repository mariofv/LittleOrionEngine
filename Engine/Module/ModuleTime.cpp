#include "ModuleTime.h"
#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "UI/EngineLog.h"
#include "Helper/Timer.h"

#include <SDL/SDL.h>

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

	APP_LOG_SUCCESS("Engine clocks initialized correctly");

	return true;
}

update_status ModuleTime::PreUpdate()
{
	frame_start_time = game_time_clock->Read();
	real_frame_start_time = real_time_clock->Read();
	return update_status::UPDATE_CONTINUE;
}


void ModuleTime::EndFrame()
{
	++frame_count;

	delta_time = (game_time_clock->Read() - frame_start_time) * time_scale;
	real_time_delta_time = real_time_clock->Read() - real_frame_start_time;

	if (limit_fps)
	{
		float remaining_frame_time = 1000.f / max_fps - real_time_delta_time;
		if (remaining_frame_time > 0)
		{
			SDL_Delay(static_cast<Uint32>(remaining_frame_time));
			last_frame_delay = remaining_frame_time;
		}
		delta_time = (game_time_clock->Read() - frame_start_time) * time_scale;
		real_time_delta_time = real_time_clock->Read() - real_frame_start_time;
	}
	else
	{
		last_frame_delay = 0.f;
	}

	time += delta_time;
	real_time_since_startup += real_time_delta_time;

	App->engine_log->LogFPS(1000 / real_time_delta_time);
	App->engine_log->LogMS(real_time_delta_time);

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

void ModuleTime::SetMaxFPS(int fps)
{
	max_fps = fps;
}

void ModuleTime::Play()
{
	if (!game_time_clock->Started())
	{
		App->editor->SaveScene(TMP_SCENE_PATH);
		game_time_clock->Start();
	}
	else
	{
		game_time_clock->Stop();
		App->editor->OpenScene(TMP_SCENE_PATH);
		remove(TMP_SCENE_PATH);
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