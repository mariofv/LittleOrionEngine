#include "PanelConfiguration.h"

#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleWindow.h"

#include "Component/ComponentCamera.h"


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

		ShowHardware();

		ImGui::Spacing();
		ShowWindowOptions();

		ImGui::Spacing();
		ShowRenderOptions();

		ImGui::Spacing();
		ShowTimeOptions();

		ImGui::Spacing();
		ShowInputOptions();
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

		int vram_budget, vram_available;
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

		sprintf(frame_info, "FPS: %.0f Frame Count: %d", 1000.f / App->time->real_time_delta_time, App->time->frame_count);
		ImGui::Text(frame_info);

		std::vector<float> ms_data = App->engine_log->getMSData();
		std::vector<float> frame_data = App->engine_log->getFPSData();

		ImGui::PlotLines("Miliseconds", &ms_data[0], ms_data.size(), 0, nullptr, 0, 80);
		ImGui::PlotLines("Frame Rate", &frame_data[0], frame_data.size(), 20, nullptr, 0, 80);
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
				ImGui::Text("Mouse pos: <INVALID>");
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

		if(ImGui::TreeNode("Game Input"))
		{

			ImGui::InputText("Name: ", &name_game_input);

			ImGui::Separator();

			ImGui::Text("Keys:");
			for(auto key : string_keys)
			{
				ImGui::Text(game_inputs_strings[key]);
			}

			ImGui::Separator();

			static const char* item_current = game_inputs_strings[0];
			if (ImGui::BeginCombo("KeyCode", item_current))
			{

				unsigned int offset = 4;
				for (int n = 0; n < game_inputs_strings.size(); ++n)
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
				keys.insert((int)selected_key);
				string_keys.insert(selected_combo);
			}

			ImGui::SameLine();

			if(ImGui::Button("Delete Keycodes"))
			{
				keys.clear();
			}

			ImGui::Separator();

			ImGui::Text("Mouse:");
			for (auto mouse_key : mouse_keys)
			{
				ImGui::Text(mouse_keys_string[(int)mouse_key]);
			}

			ImGui::Separator();

			static const char* mouse_current = mouse_keys_string[0];
			if (ImGui::BeginCombo("MouseCode", mouse_current))
			{
				for (int n = 0; n < mouse_keys_string.size(); ++n)
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
				mouse_keys.insert((int)selected_mouse);
			}

			ImGui::SameLine();

			if (ImGui::Button("Delete Mouse Buttons"))
			{
				mouse_keys.clear();
			}

			ImGui::Separator();

			if(ImGui::Button("Create Game Input"))
			{
				GameInput game_input;
				game_input.name = name_game_input;
				for(auto key : keys)
				{
					game_input.keys.push_back((KeyCode)key);
				}

				for (auto mouse : mouse_keys)
				{
					game_input.mouse_buttons.push_back((MouseButton)mouse);
				}

				App->input->CreateGameInput(game_input);
			}

			ImGui::TreePop();
		}

		
		
	}
}