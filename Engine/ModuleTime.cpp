#include "Globals.h"
#include "Application.h"
#include "ModuleTime.h"
#include "EngineLog.h"
#include "Timer.h"

#include "imgui.h"

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

	App->engine_log->logFPS(1000/real_time_delta_time);
	App->engine_log->logMS(real_time_delta_time);

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

void ModuleTime::SetMaxFPS(const int fps)
{
	max_fps = fps;
}

void ModuleTime::SetTimeScale(const float time_scale)
{
	this->time_scale = time_scale;
}

void ModuleTime::ShowTimeOptions()
{
	if (ImGui::CollapsingHeader("Timers"))
	{
		if (ImGui::SliderInt("Max FPS", &max_fps, 10, 60))
		{
			SetMaxFPS(max_fps);
		}

		char frame_info[2048];
		sprintf(frame_info, "Limiting to %d fps means each frame needs to take %f ms", max_fps, 1000.f / max_fps);
		ImGui::Text(frame_info);

		sprintf_s(frame_info, "Real Time since Start: %.0f Real Time dt: %.0f", RealTimeSinceStartup(), real_time_delta_time);
		ImGui::Text(frame_info);

		sprintf_s(frame_info, "Game Time since Start: %.0f Game Time dt: %.0f", Time(), delta_time);
		ImGui::Text(frame_info);

		if (ImGui::SliderFloat("Game Clock Scale", &time_scale, 0.5, 2))
		{
			SetTimeScale(time_scale);
		}

		sprintf(frame_info, "FPS: %.0f Frame Count: %d", 1000.f/real_time_delta_time, frame_count);
		ImGui::Text(frame_info);

		std::vector<float> ms_data = App->engine_log->getMSData();
		std::vector<float> frame_data = App->engine_log->getFPSData();

		ImGui::PlotLines("Miliseconds", &ms_data[0], ms_data.size(), 0 , nullptr, 0, (1000.f / max_fps) * 2);
		ImGui::PlotLines("Frame Rate", &frame_data[0], frame_data.size(), 20, nullptr, max_fps*0.75, max_fps*1.25);
	}
}

