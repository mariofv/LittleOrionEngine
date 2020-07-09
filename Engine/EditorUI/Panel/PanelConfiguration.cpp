#include "Component/ComponentCamera.h"

#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleEffects.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleSpacePartitioning.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleWindow.h"
#include "PanelConfiguration.h"
#include "Module/ModulePhysics.h"

#include <FontAwesome5/IconsFontAwesome5.h>
#include <GL/glew.h>
#include <imgui.h>
#include <imgui_stdlib.h>
#include <SDL/SDL.h>

PanelConfiguration::PanelConfiguration()
{
	window_name = ICON_FA_COGS " Configuration";
}

void PanelConfiguration::Render()
{
	if (ImGui::Begin(ICON_FA_COGS " Configuration", &opened))
	{
		hovered = ImGui::IsWindowHovered();
		focused = ImGui::IsWindowFocused();

		ShowHardware();

		ImGui::Spacing();
		ShowWindowOptions();

		ImGui::Spacing();
		ShowRenderOptions();

		ImGui::Spacing();
		ShowTimeOptions();

		ImGui::Spacing();
		ShowInputOptions();

		ImGui::Spacing();
		ShowPhysicsOptions();
		
		ImGui::Spacing();
		ShowSpacePartitioningOptions();
	}
	ImGui::End();
}

void PanelConfiguration::ShowHardware() const
{
	if (ImGui::CollapsingHeader(ICON_FA_HDD " Hardware"))
	{
		char tmp_string[4096];

		ImGui::Text("CPUs:");
		ImGui::SameLine();
		int num_cpus = SDL_GetCPUCount();
		int cpu_cache_line_size = SDL_GetCPUCacheLineSize();
		sprintf_s(tmp_string, "%d (Cache: %d bytes)", num_cpus, cpu_cache_line_size);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("System RAM:");
		ImGui::SameLine();
		float system_ram = SDL_GetSystemRAM() / 1000.f;
		sprintf_s(tmp_string, "%.2f GB", system_ram);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("Caps:");
		if (SDL_Has3DNow()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "3DNow");
		}
		if (SDL_HasAVX()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX");
		}
		if (SDL_HasAVX2()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "AVX2");
		}
		if (SDL_HasMMX()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "MMX");
		}
		if (SDL_HasRDTSC()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "RDTSC");
		}
		if (SDL_HasSSE()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE");
		}
		if (SDL_HasSSE2()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE2");
		}
		if (SDL_HasSSE3()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE3");
		}
		if (SDL_HasSSE41()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE41");
		}
		if (SDL_HasSSE42()) {
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "SSE42");
		}

		ImGui::Separator();

		ImGui::Text("GPU:");
		ImGui::SameLine();
		sprintf_s(tmp_string, "%s %s", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		int vram_budget;
		int vram_available;
		glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &vram_budget);
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &vram_available);

		ImGui::Text("VRAM Budget:");
		ImGui::SameLine();
		sprintf_s(tmp_string, "%.2f MB", vram_budget / 1000.f);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("VRAM Usage:");
		ImGui::SameLine();
		sprintf_s(tmp_string, "%.2f MB", (vram_budget - vram_available) / 1000.f);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);

		ImGui::Text("VRAM Available:");
		ImGui::SameLine();
		sprintf_s(tmp_string, "%.2f MB", vram_available / 1000.f);
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), tmp_string);
	}
}


void PanelConfiguration::ShowWindowOptions()
{
	ImGui::PushFont(App->editor->GetFont(Fonts::FONT_FAR));
	if (ImGui::CollapsingHeader(ICON_FA_WINDOW_MAXIMIZE " Window")) {
		if (ImGui::SliderFloat("Brightness", &App->window->brightness, 0, 1))
		{
			App->window->SetBrightness(App->window->brightness);
		}

		if (ImGui::SliderInt("Width", &App->window->width, App->window->screen_width / 4, App->window->screen_width))
		{
			App->window->SetWidth(App->window->width);
		}

		if (ImGui::SliderInt("Height", &App->window->height, App->window->screen_height / 4, App->window->screen_height))
		{
			App->window->SetHeight(App->window->height);
		}

		if (ImGui::Combo("Window style", &App->window->fullscreen, "Windowed\0Fullscreen desktop\0Fullscreen\0"))
		{
			switch (App->window->fullscreen)
			{
			case 0:
				App->window->SetWindowed();
				break;
			case 1:
				App->window->SetFullScreenDesktop();
				break;
			case 2:
				App->window->SetFullScreen();
				break;
			}
		}

		if (ImGui::Checkbox("Bordered", &App->window->bordered))
		{
			App->window->SetBordered(App->window->bordered);
		}
		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &App->window->resizable))
		{
			App->window->SetResizable(App->window->resizable);
		}
	}
	ImGui::PopFont();
}

void PanelConfiguration::ShowRenderOptions()
{
	if (ImGui::CollapsingHeader(ICON_FA_CLONE " Renderer"))
	{
		if (ImGui::Checkbox("VSync", &App->renderer->vsync))
		{
			App->renderer->SetVSync(App->renderer->vsync);
		}

		ImGui::SameLine();
		if (ImGui::Checkbox("Depth test", &App->renderer->gl_depth_test))
		{
			App->renderer->SetDepthTest(App->renderer->gl_depth_test);

		}

		ImGui::SameLine();
		if (ImGui::Checkbox("Anti-aliasing", &App->renderer->anti_aliasing))
		{
			App->cameras->scene_camera->toggle_msaa = true;
			App->cameras->main_camera->toggle_msaa = true;
		}

		ImGui::Separator();

		if (ImGui::Checkbox("Face culling", &App->renderer->gl_cull_face))
		{
			App->renderer->SetFaceCulling(App->renderer->gl_cull_face);
		}
		if (ImGui::Combo("Culled faces", &App->renderer->culled_faces, "Back\0Front\0Front and back\0"))
		{
			switch (App->renderer->culled_faces)
			{
			case 0:
				App->renderer->SetCulledFaces(GL_BACK);
				break;
			case 1:
				App->renderer->SetCulledFaces(GL_FRONT);
				break;
			case 2:
				App->renderer->SetCulledFaces(GL_FRONT_AND_BACK);
				break;
			}
		}
		if (ImGui::Combo("Front faces", &App->renderer->front_faces, "Counterclockwise\0Clockwise\0"))
		{
			switch (App->renderer->front_faces)
			{
			case 0:
				App->renderer->SetFrontFaces(GL_CCW);
				break;
			case 1:
				App->renderer->SetFrontFaces(GL_CW);
				break;
			}
		}
		if (ImGui::Combo("Filling mode", &App->renderer->filling_mode, "Fill\0Lines\0Vertices"))
		{
			switch (App->renderer->filling_mode) {
			case 0:
				glPolygonMode(GL_FRONT, GL_FILL);
				break;
			case 1:
				glPolygonMode(GL_FRONT, GL_LINE);
				break;
			case 2:
				glPolygonMode(GL_FRONT, GL_POINT);
				break;
			}
		}
		ImGui::Separator();
		HelpMarker("This settings have no visual impact, WIP.");
		ImGui::SameLine();
		if (ImGui::TreeNode("Non-functional settings"))
		{
			if (ImGui::Checkbox("Alpha test", &App->renderer->gl_alpha_test))
			{
				App->renderer->SetAlphaTest(App->renderer->gl_alpha_test);
			}
			if (ImGui::Checkbox("Scissor test", &App->renderer->gl_scissor_test))
			{
				App->renderer->SetScissorTest(App->renderer->gl_scissor_test);
			}
			if (ImGui::Checkbox("Stencil test", &App->renderer->gl_stencil_test))
			{
				App->renderer->SetStencilTest(App->renderer->gl_stencil_test);
			}
			if (ImGui::Checkbox("Blending", &App->renderer->gl_blend))
			{
				App->renderer->SetBlending(App->renderer->gl_blend);
			}
			if (ImGui::Checkbox("Dithering", &App->renderer->gl_dither))
			{
				App->renderer->SetDithering(App->renderer->gl_dither);
			}
			if (ImGui::Checkbox("Min Maxing", &App->renderer->gl_minmax))
			{
				App->renderer->SetMinMaxing(App->renderer->gl_minmax);
			}
			ImGui::TreePop();
		}

		ImGui::Separator();

		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Lighting");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 1, 1), "and");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 1), "Shadows");

		ImGui::Separator();

		ImGui::TextColored(ImVec4(1, 1, 0, 1), "Ambient Light");

		ImGui::SliderFloat("Intensity", &App->lights->ambient_light_intensity, 0, 1, "%.2f");
		ImGui::ColorEdit3("Color", App->lights->ambient_light_color);

		//ImGui::Checkbox("Toggle directional camera frustum", &App->renderer->toggle_ortho_frustum);

		ImGui::Checkbox("Render shadows", &App->lights->render_shadows);
		ImGui::SliderFloat("Shadows Fov Factor", &App->lights->main_camera_fov_increment_factor, 0, 4, "%.2f");

		ImGui::Checkbox("Render Particles", &App->effects->render_particles);


	}
}

void PanelConfiguration::ShowTimeOptions()
{
	if (ImGui::CollapsingHeader(ICON_FA_CLOCK " Timers"))
	{
		ImGui::Checkbox("", &App->time->limit_fps);

		ImGui::SameLine();

		if (ImGui::SliderInt("Max FPS", &App->time->max_fps, 10, 60))
		{
			App->time->SetMaxFPS(App->time->max_fps);
		}

		char frame_info[2048];
		sprintf(frame_info, "Limiting to %d fps means each frame needs to take %f ms", App->time->max_fps, 1000.f / App->time->max_fps);
		ImGui::Text(frame_info);

		sprintf(frame_info, "Last frame we delayed for %f ms", App->time->last_frame_delay);
		ImGui::Text(frame_info);

		ImGui::Separator();

		if (ImGui::SliderFloat("Game Clock Scale", &App->time->time_scale, 0.5, 2))
		{
			App->time->SetTimeScale(App->time->time_scale);
		}

		sprintf_s(frame_info, "Real Time since Start: %.0f Real Time dt: %.0f", App->time->real_time_since_startup, App->time->real_time_delta_time);
		ImGui::Text(frame_info);

		sprintf_s(frame_info, "Game Time since Start: %.0f Game Time dt: %.0f", App->time->time, App->time->delta_time);
		ImGui::Text(frame_info);

		ImGui::Separator();

		sprintf(frame_info, "FPS: %.0f Frame Count: %d", App->time->current_fps, App->time->frame_count);
		ImGui::Text(frame_info);

	}

}

void PanelConfiguration::ShowInputOptions()
{
	if (ImGui::CollapsingHeader(ICON_FA_KEYBOARD " Input"))
	{
		ImGuiIO& io = ImGui::GetIO();

		// Display ImGuiIO output flags
		ImGui::Text("WantCaptureMouse: %d", io.WantCaptureMouse);
		ImGui::Text("WantCaptureKeyboard: %d", io.WantCaptureKeyboard);
		ImGui::Text("WantTextInput: %d", io.WantTextInput);
		ImGui::Text("WantSetMousePos: %d", io.WantSetMousePos);
		ImGui::Text("NavActive: %d, NavVisible: %d", io.NavActive, io.NavVisible);

		// Display Keyboard/Mouse state
		if (ImGui::TreeNode("Keyboard, Mouse & Navigation State"))
		{

			float2 mouse_pos = App->input->GetMousePosition();
			float2 mouse_rel = App->input->GetMouseMotion();
			if (ImGui::IsMousePosValid())
			{
				ImGui::Text("Mouse pos (SDL): (%g, %g)", mouse_pos.x, mouse_pos.y);
				ImGui::Text("Mouse pos (IMGUI): (%g, %g)", io.MousePos.x, io.MousePos.y);
			}
			else
			{
				ImGui::Text("Mouse pos: <INVALID>");
			}
			ImGui::Text("Mouse delta (SDL):(%g, %g)", mouse_rel.x, mouse_rel.y);
			ImGui::Text("Mouse delta (IMGUI): (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
			ImGui::Text("Mouse down:");     for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (io.MouseDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
			ImGui::Text("Mouse clicked:");  for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
			ImGui::Text("Mouse dbl-clicked:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseDoubleClicked(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
			ImGui::Text("Mouse released:"); for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) if (ImGui::IsMouseReleased(i)) { ImGui::SameLine(); ImGui::Text("b%d", i); }
			ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);

			ImGui::Text("Keys down:");      for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (io.KeysDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("%d (0x%X) (%.02f secs)", i, i, io.KeysDownDuration[i]); }
			ImGui::Text("Keys pressed:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyPressed(i)) { ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i); }
			ImGui::Text("Keys release:");   for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) if (ImGui::IsKeyReleased(i)) { ImGui::SameLine(); ImGui::Text("%d (0x%X)", i, i); }
			ImGui::Text("Keys mods: %s%s%s%s", io.KeyCtrl ? "CTRL " : "", io.KeyShift ? "SHIFT " : "", io.KeyAlt ? "ALT " : "", io.KeySuper ? "SUPER " : "");
			ImGui::Text("Chars queue:");    for (int i = 0; i < io.InputQueueCharacters.Size; i++) { ImWchar c = io.InputQueueCharacters[i]; ImGui::SameLine();  ImGui::Text("\'%c\' (0x%04X)", (c > ' ' && c <= 255) ? (char)c : '?', c); } // FIXME: We should convert 'c' to UTF-8 here but the functions are not public.

			ImGui::Text("NavInputs down:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputs[i] > 0.0f) { ImGui::SameLine(); ImGui::Text("[%d] %.2f", i, io.NavInputs[i]); }
			ImGui::Text("NavInputs pressed:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] == 0.0f) { ImGui::SameLine(); ImGui::Text("[%d]", i); }
			ImGui::Text("NavInputs duration:"); for (int i = 0; i < IM_ARRAYSIZE(io.NavInputs); i++) if (io.NavInputsDownDuration[i] >= 0.0f) { ImGui::SameLine(); ImGui::Text("[%d] %.2f", i, io.NavInputsDownDuration[i]); }

			ImGui::Separator();

			ImGui::Text("Number of controllers connected: %d", App->input->total_game_controllers);
			ImGui::Separator();
			if(App->input->total_game_controllers > 0)
			{
				ImGui::Text("Controller 1:");
				ImGui::Separator();
				ImGui::Text("A: %d", App->input->GetControllerButton(ControllerCode::A, ControllerID::ONE));
				ImGui::Text("B: %d", App->input->GetControllerButton(ControllerCode::B, ControllerID::ONE));
				ImGui::Text("X: %d", App->input->GetControllerButton(ControllerCode::X, ControllerID::ONE));
				ImGui::Text("Y: %d", App->input->GetControllerButton(ControllerCode::Y, ControllerID::ONE));
				ImGui::Text("Back: %d", App->input->GetControllerButton(ControllerCode::Back, ControllerID::ONE));
				ImGui::Text("DownDpad: %d", App->input->GetControllerButton(ControllerCode::DownDpad, ControllerID::ONE));
				ImGui::Text("Guide: %d", App->input->GetControllerButton(ControllerCode::Guide, ControllerID::ONE));
				ImGui::Text("LeftDpad: %d", App->input->GetControllerButton(ControllerCode::LeftDpad, ControllerID::ONE));
				ImGui::Text("LeftShoulder: %d", App->input->GetControllerButton(ControllerCode::LeftShoulder, ControllerID::ONE));
				ImGui::Text("LeftStick: %d", App->input->GetControllerButton(ControllerCode::LeftStick, ControllerID::ONE));
				ImGui::Text("RightDpad: %d", App->input->GetControllerButton(ControllerCode::RightDpad, ControllerID::ONE));
				ImGui::Text("RightShoulder: %d", App->input->GetControllerButton(ControllerCode::RightShoulder, ControllerID::ONE));
				ImGui::Text("RightStick: %d", App->input->GetControllerButton(ControllerCode::RightStick, ControllerID::ONE));
				ImGui::Text("Start: %d", App->input->GetControllerButton(ControllerCode::Start, ControllerID::ONE));
				ImGui::Text("UpDpad: %d", App->input->GetControllerButton(ControllerCode::UpDpad, ControllerID::ONE));

				ImGui::Text("Left Joystick : (%.3f, %.3f)", App->input->GetAxisController(ControllerAxis::LEFT_JOYSTICK, ControllerID::ONE).x, App->input->GetAxisController(ControllerAxis::LEFT_JOYSTICK, ControllerID::ONE).y);
				ImGui::Text("Right Joystick : (%.3f, %.3f)", App->input->GetAxisController(ControllerAxis::RIGHT_JOYSTICK, ControllerID::ONE).x, App->input->GetAxisController(ControllerAxis::RIGHT_JOYSTICK, ControllerID::ONE).y);

				ImGui::Text("Left Joystick Raw : (%.3f, %.3f)", App->input->GetAxisController(ControllerAxis::LEFT_JOYSTICK_RAW, ControllerID::ONE).x, App->input->GetAxisController(ControllerAxis::LEFT_JOYSTICK_RAW, ControllerID::ONE).y);
				ImGui::Text("Right Joystick Raw : (%.3f, %.3f)", App->input->GetAxisController(ControllerAxis::RIGHT_JOYSTICK_RAW, ControllerID::ONE).x, App->input->GetAxisController(ControllerAxis::RIGHT_JOYSTICK_RAW, ControllerID::ONE).y);

				ImGui::Text("Left Trigger : %.3f", App->input->GetTriggerController(ControllerAxis::LEFT_TRIGGER, ControllerID::ONE));
				ImGui::Text("Right Trigger : %.3f", App->input->GetTriggerController(ControllerAxis::RIGHT_TRIGGER, ControllerID::ONE));

				ImGui::Text("Left Raw Trigger : %d", App->input->GetTriggerControllerRaw(ControllerAxis::LEFT_TRIGGER_RAW, ControllerID::ONE));
				ImGui::Text("Right Raw Trigger : %d", App->input->GetTriggerControllerRaw(ControllerAxis::RIGHT_TRIGGER_RAW, ControllerID::ONE));
			}

			ImGui::Separator();
			if(App->input->total_game_controllers > 1)
			{
				ImGui::Text("Controller 2:");
				ImGui::Separator();
				ImGui::Text("A: %d", App->input->GetControllerButton(ControllerCode::A, ControllerID::TWO));
				ImGui::Text("B: %d", App->input->GetControllerButton(ControllerCode::B, ControllerID::TWO));
				ImGui::Text("X: %d", App->input->GetControllerButton(ControllerCode::X, ControllerID::TWO));
				ImGui::Text("Y: %d", App->input->GetControllerButton(ControllerCode::Y, ControllerID::TWO));
				ImGui::Text("Back: %d", App->input->GetControllerButton(ControllerCode::Back, ControllerID::TWO));
				ImGui::Text("DownDpad: %d", App->input->GetControllerButton(ControllerCode::DownDpad, ControllerID::TWO));
				ImGui::Text("Guide: %d", App->input->GetControllerButton(ControllerCode::Guide, ControllerID::TWO));
				ImGui::Text("LeftDpad: %d", App->input->GetControllerButton(ControllerCode::LeftDpad, ControllerID::TWO));
				ImGui::Text("LeftShoulder: %d", App->input->GetControllerButton(ControllerCode::LeftShoulder, ControllerID::TWO));
				ImGui::Text("LeftStick: %d", App->input->GetControllerButton(ControllerCode::LeftStick, ControllerID::TWO));
				ImGui::Text("RightDpad: %d", App->input->GetControllerButton(ControllerCode::RightDpad, ControllerID::TWO));
				ImGui::Text("RightShoulder: %d", App->input->GetControllerButton(ControllerCode::RightShoulder, ControllerID::TWO));
				ImGui::Text("RightStick: %d", App->input->GetControllerButton(ControllerCode::RightStick, ControllerID::TWO));
				ImGui::Text("Start: %d", App->input->GetControllerButton(ControllerCode::Start, ControllerID::TWO));
				ImGui::Text("UpDpad: %d", App->input->GetControllerButton(ControllerCode::UpDpad, ControllerID::TWO));

				ImGui::Text("Left Joystick : (%.3f, %.3f)", App->input->GetAxisController(ControllerAxis::LEFT_JOYSTICK, ControllerID::TWO).x, App->input->GetAxisController(ControllerAxis::LEFT_JOYSTICK, ControllerID::TWO).y);
				ImGui::Text("Right Joystick : (%.3f, %.3f)", App->input->GetAxisController(ControllerAxis::RIGHT_JOYSTICK, ControllerID::TWO).x, App->input->GetAxisController(ControllerAxis::RIGHT_JOYSTICK, ControllerID::TWO).y);

				ImGui::Text("Left Joystick Raw : (%.3f, %.3f)", App->input->GetAxisController(ControllerAxis::LEFT_JOYSTICK_RAW, ControllerID::TWO).x, App->input->GetAxisController(ControllerAxis::LEFT_JOYSTICK_RAW, ControllerID::TWO).y);
				ImGui::Text("Right Joystick Raw : (%.3f, %.3f)", App->input->GetAxisController(ControllerAxis::RIGHT_JOYSTICK_RAW, ControllerID::TWO).x, App->input->GetAxisController(ControllerAxis::RIGHT_JOYSTICK_RAW, ControllerID::TWO).y);

				ImGui::Text("Left Trigger : %.3f", App->input->GetTriggerController(ControllerAxis::LEFT_TRIGGER, ControllerID::TWO));
				ImGui::Text("Right Trigger : %.3f", App->input->GetTriggerController(ControllerAxis::RIGHT_TRIGGER, ControllerID::TWO));

				ImGui::Text("Left Raw Trigger : %d", App->input->GetTriggerControllerRaw(ControllerAxis::LEFT_TRIGGER_RAW, ControllerID::TWO));
				ImGui::Text("Right Raw Trigger : %d", App->input->GetTriggerControllerRaw(ControllerAxis::RIGHT_TRIGGER_RAW, ControllerID::TWO));
			}
			
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Dragging"))
		{
			for (int button = 0; button < 3; button++)
				ImGui::Text("IsMouseDragging(%d):\n  w/ default threshold: %d,\n  w/ zero threshold: %d\n  w/ large threshold: %d",
					button, ImGui::IsMouseDragging(button), ImGui::IsMouseDragging(button, 0.0f), ImGui::IsMouseDragging(button, 20.0f));

			ImVec2 value_raw = ImGui::GetMouseDragDelta(0, 0.0f);
			ImVec2 value_with_lock_threshold = ImGui::GetMouseDragDelta(0);
			ImVec2 mouse_delta = io.MouseDelta;
			ImGui::Text("GetMouseDragDelta(0):\n  w/ default threshold: (%.1f, %.1f),\n  w/ zero threshold: (%.1f, %.1f)\nMouseDelta: (%.1f, %.1f)", value_with_lock_threshold.x, value_with_lock_threshold.y, value_raw.x, value_raw.y, mouse_delta.x, mouse_delta.y);
			ImGui::TreePop();
		}

		if(ImGui::TreeNode("Create Game Input"))
		{

			ImGui::InputText("Name: ", &name_game_input);

			ImGui::Separator();

			ImGui::Text("Keys:");
			for(const auto key : string_keys)
			{
				ImGui::Text(game_inputs_strings[key]);
			}

			ImGui::Separator();

			static const char* item_current = game_inputs_strings[0];
			if (ImGui::BeginCombo("KeyCode", item_current))
			{

				unsigned int offset = 4;
				for (unsigned int n = 0; n < game_inputs_strings.size(); ++n)
				{
					//Handle offset
					if (n > FIRST_OFFSET_COND)
						offset = FIRST_OFFSET;
					else if (n > SECOND_OFFSET_COND)
						offset = SECOND_OFFSET;
					else if (n > THIRD_OFFSET_COND)
						offset = THIRD_OFFSET;

					bool is_selected = (item_current == game_inputs_strings[n]);
					if (ImGui::Selectable(game_inputs_strings[n], is_selected))
					{
						item_current = game_inputs_strings[n];
						selected_key = KeyCode(n + offset);
						selected_combo = n;
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
				ImGui::EndCombo();
			}

			if(ImGui::Button("Add KeyCode"))
			{
				keys.insert(static_cast<int>(selected_key));
				string_keys.insert(selected_combo);
			}

			ImGui::SameLine();

			if(ImGui::Button("Delete Keycodes"))
			{
				keys.clear();
				string_keys.clear();
			}

			ImGui::Separator();

			ImGui::Text("Mouse:");
			for (const auto mouse_key : mouse_keys)
			{
				ImGui::Text(mouse_keys_string[static_cast<int>(mouse_key)]);
			}

			ImGui::Separator();

			static const char* mouse_current = mouse_keys_string[0];
			if (ImGui::BeginCombo("MouseCode", mouse_current))
			{
				for (unsigned int n = 0; n < mouse_keys_string.size(); ++n)
				{
					bool is_selected = (mouse_current == mouse_keys_string[n]);
					if (ImGui::Selectable(mouse_keys_string[n], is_selected))
					{
						mouse_current = mouse_keys_string[n];
						selected_mouse = MouseButton(n);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Add Mouse Button"))
			{
				mouse_keys.insert(static_cast<int>(selected_mouse));
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Mouse Buttons"))
			{
				mouse_keys.clear();
			}

			ImGui::Separator();


			ImGui::Text("Controller Keys:");
			for (const auto controller_key : controller_keys)
			{
				ImGui::Text(controller_keys_string[static_cast<int>(controller_key)]);
			}

			ImGui::Separator();

			static const char* controller_current = controller_keys_string[0];
			if (ImGui::BeginCombo("ControllerCodes", controller_current))
			{
				for (unsigned int n = 0; n < controller_keys_string.size(); ++n)
				{
					bool is_selected = (controller_current == controller_keys_string[n]);
					if (ImGui::Selectable(controller_keys_string[n], is_selected))
					{
						controller_current = controller_keys_string[n];
						selected_controller = ControllerCode(n);
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("Add Controller Button"))
			{
				controller_keys.insert(static_cast<int>(selected_controller));
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Controller Buttons"))
			{
				controller_keys.clear();
			}

			ImGui::Separator();

			if(ImGui::Button("Create Game Input"))
			{
				GameInput game_input = GameInput();
				game_input.name = name_game_input;
				int i = 0;
				for(const auto key : keys)
				{
					game_input.keys[i] = ((KeyCode)key);
					++i;
				}
				int j = 0;
				for (const auto mouse : mouse_keys)
				{
					game_input.mouse_buttons[j] = ((MouseButton)mouse);
					++j;
				}
				int k = 0;
				for (const auto controller_key : controller_keys)
				{
					game_input.controller_buttons[k] = ((ControllerCode)controller_key);
					++k;
				}

				App->input->CreateGameInput(game_input);
			}

			ImGui::TreePop();
		}

		
		if (ImGui::TreeNode("See Game Inputs"))
		{

			ImGui::Text("Game Inputs:");

			ImGui::Separator();

			for(const auto& game_input : App->input->game_inputs)
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", game_input.first.c_str());
				ImGui::Text("KeyCodes:");
				for(const auto& key : game_input.second.keys)
				{
					
					if((uint64_t)key <= 0)
					{
						continue;
					}

					size_t aux = static_cast<size_t>(key);
					if (aux > FIRST_OFFSET_COND)
						aux -= FIRST_OFFSET;
					else if (aux > SECOND_OFFSET_COND)
						aux -= SECOND_OFFSET;
					else if (aux > THIRD_OFFSET_COND)
						aux -= THIRD_OFFSET;
					else
						aux -= 4;


					ImGui::Text("	%s", game_inputs_strings[aux]);
				}
				ImGui::Text("MouseCodes:");
				for (auto& mouse : game_input.second.mouse_buttons)
				{
					if ((uint64_t)mouse <= 0)
					{
						continue;
					}

					ImGui::Text("	%s", mouse_keys_string[static_cast<int>(mouse)]);
				}
				ImGui::Text("ControllerCodes:");
				for (const auto& controller_key : game_input.second.controller_buttons)
				{
					if ((uint64_t)controller_key > 17)
					{
						continue;
					}

					ImGui::Text("	%s", controller_keys_string[static_cast<int>(controller_key)]);
				}
				
				
				if(ImGui::Button("Delete GameInputs"))
				{
					App->input->DeleteGameInput(game_input.second);
				}

				ImGui::Separator();

			}
		
			ImGui::TreePop();
		}
	}
}

void PanelConfiguration::ShowPhysicsOptions()
{
	ImGui::PushFont(App->editor->GetFont(Fonts::FONT_FAR));
	if (ImGui::CollapsingHeader(ICON_FA_WINDOW_MAXIMIZE " Physics")) {

		if (ImGui::DragFloat3("Gravity", &App->physics->gravity.x, -1000, 1000))
		{
			App->physics->SetGravity(App->physics->GetGravity());
		}

		ImGui::Text("Consumption (ms)");
		ImGui::PlotLines("Miliseconds", &App->physics->ms_info[0], App->physics->ms_info.size(), 0, nullptr, 0, 80);
	}
	ImGui::PopFont();
}
	
void PanelConfiguration::ShowSpacePartitioningOptions()
{
	if (ImGui::CollapsingHeader(ICON_FA_TREE " SpacePartitioning"))
	{
		ImGui::Checkbox("Scene window culling", &App->debug->culling_scene_mode);
		int culling_mode_int = static_cast<int>(App->debug->culling_mode);
		if (ImGui::Combo("Culling Mode", &culling_mode_int, "None\0Frustum Culling\0QuadTree Culling\0OctTree Culling\0AabbTree Culling\0Combined Culling"))
		{
			switch (culling_mode_int)
			{
			case 0:
				App->debug->culling_mode = ModuleDebug::CullingMode::NONE;
				break;
			case 1:
				App->debug->culling_mode = ModuleDebug::CullingMode::FRUSTUM_CULLING;
				break;
			case 2:
				App->debug->culling_mode = ModuleDebug::CullingMode::QUADTREE_CULLING;
				break;
			case 3:
				App->debug->culling_mode = ModuleDebug::CullingMode::OCTTREE_CULLING;
				break;
			case 4:
				App->debug->culling_mode = ModuleDebug::CullingMode::AABBTREE_CULLING;
				break;
			case 5:
				App->debug->culling_mode = ModuleDebug::CullingMode::COMBINED_CULLING;
				break;
			}
		}

		ImGui::Spacing();
		ImGui::Text("Space Partitioning");
		if (ImGui::SliderInt("Quadtree Depth ", &App->space_partitioning->ol_quadtree->max_depth, 1, 10))
		{
			App->space_partitioning->GenerateQuadTree();
		}
		if (ImGui::SliderInt("Quadtree bucket size ", &App->space_partitioning->ol_quadtree->bucket_size, 1, 10))
		{
			App->space_partitioning->GenerateQuadTree();
		}

		if (ImGui::Button("Generate QuadTree"))
		{
			App->space_partitioning->GenerateQuadTree();
		}

		ImGui::Separator();

		if (ImGui::SliderInt("OctTree Depth ", &App->space_partitioning->ol_octtree->max_depth, 1, 10))
		{
			App->space_partitioning->GenerateOctTree();
		}
		if (ImGui::SliderInt("OctTree bucket size ", &App->space_partitioning->ol_octtree->bucket_size, 1, 10))
		{
			App->space_partitioning->GenerateOctTree();
		}

		if (ImGui::Button("Generate OctTree"))
		{
			App->space_partitioning->GenerateOctTree();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::CollapsingHeader("Frustum culling scene"))
		{
			ImGui::SliderInt("Number of objects", &App->debug->num_houses, 0, 1000);
			ImGui::SliderInt("Dispersion X", &App->debug->max_dispersion_x, 0, 1000);
			ImGui::SliderInt("Dispersion Z", &App->debug->max_dispersion_z, 0, 1000);
			if (ImGui::Button("Create scene"))
			{
				App->debug->CreateFrustumCullingDebugScene();
			}
		}
		
	}

}
