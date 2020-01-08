#include "ModuleTime.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "UI/EngineUI.h"
#include "UI/EngineLog.h"
#include "Timer.h"

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>

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

void ModuleTime::ShowTimeControls()
{
	ImVec2 time_window_size = ImGui::GetWindowSize();

	ImVec2 play_button_pos((time_window_size.x - 24)*0.5f - 26, (time_window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(play_button_pos);
	if (game_time_clock->Started())
	{
		// CHANGE DEFAULT BUTTON COLOR TO SELECTED BUTTON COLOR
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5816f, 0.94f, 0.9804f));
		if (ImGui::Button(ICON_FA_PLAY, ImVec2(24, 24)))
		{
			Play();
		}
		ImGui::PopStyleColor();
	}
	else
	{
		if (ImGui::Button(ICON_FA_PLAY, ImVec2(24, 24)))
		{
			Play();
		}
	}

	ImGui::SameLine();

	ImVec2 pause_button_pos((time_window_size.x - 24)*0.5f, (time_window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(pause_button_pos);
	if (game_time_clock->IsPaused())
	{
		// CHANGE DEFAULT BUTTON COLOR TO SELECTED BUTTON COLOR
		ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.5816f, 0.94f, 0.9804f)); 
		if (ImGui::Button(ICON_FA_PAUSE, ImVec2(24, 24)))
		{
			Pause();
		}
		ImGui::PopStyleColor();
	}
	else 
	{
		if (ImGui::Button(ICON_FA_PAUSE, ImVec2(24, 24)))
		{
			Pause();
		}
	}
		
	ImGui::SameLine();

	ImVec2 step_button_pos((time_window_size.x - 24)*0.5f + 26, (time_window_size.y - 24)*0.5f);
	ImGui::SetCursorPos(step_button_pos);
	if (ImGui::Button(ICON_FA_STEP_FORWARD, ImVec2(24, 24)))
	{
		StepFrame();
	}
}

void ModuleTime::ShowTimeOptions()
{
	if (ImGui::CollapsingHeader(ICON_FA_CLOCK " Timers"))
	{
		ImGui::Checkbox("", &limit_fps);

		ImGui::SameLine();

		if (ImGui::SliderInt("Max FPS", &max_fps, 10, 60))
		{
			SetMaxFPS(max_fps);
		}

		char frame_info[2048];
		sprintf(frame_info, "Limiting to %d fps means each frame needs to take %f ms", max_fps, 1000.f / max_fps);
		ImGui::Text(frame_info);

		sprintf(frame_info, "Last frame we delayed for %f ms", last_frame_delay);
		ImGui::Text(frame_info);

		ImGui::Separator();

		if (ImGui::SliderFloat("Game Clock Scale", &time_scale, 0.5, 2))
		{
			SetTimeScale(time_scale);
		}

		sprintf_s(frame_info, "Real Time since Start: %.0f Real Time dt: %.0f", real_time_since_startup, real_time_delta_time);
		ImGui::Text(frame_info);

		sprintf_s(frame_info, "Game Time since Start: %.0f Game Time dt: %.0f", time, delta_time);
		ImGui::Text(frame_info);

		ImGui::Separator();

		sprintf(frame_info, "FPS: %.0f Frame Count: %d", 1000.f/real_time_delta_time, frame_count);
		ImGui::Text(frame_info);

		std::vector<float> ms_data = App->engine_log->getMSData();
		std::vector<float> frame_data = App->engine_log->getFPSData();

		ImGui::PlotLines("Miliseconds", &ms_data[0], ms_data.size(), 0 , nullptr, 0, 80);
		ImGui::PlotLines("Frame Rate", &frame_data[0], frame_data.size(), 20, nullptr, 0, 80);
	}
}

