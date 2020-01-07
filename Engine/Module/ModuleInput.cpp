#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleModelLoader.h"
#include "ModuleCamera.h"
#include "Component/ComponentCamera.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ModuleUI.h"
#include "UI/EngineUI.h"
#include "UI/FileExplorerUI.h"

#include <SDL/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include <FontAwesome5/IconsFontAwesome5.h>
#include <GL/glew.h>


// Called before render is available
bool ModuleInput::Init()
{
	APP_LOG_SECTION("************ Module Input Init ************");

	APP_LOG_INIT("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		APP_LOG_ERROR("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	APP_LOG_SUCCESS("SDL input event system initialized correctly.");

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
	SDL_PumpEvents();

	SDL_Event event;
	while (SDL_PollEvent(&event) != 0)
	{
		ImGui_ImplSDL2_ProcessEvent(&event);

		// Esc button is pressed
		switch (event.type)
		{
		case SDL_QUIT:
			return update_status::UPDATE_STOP;
			break;

		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED || event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				App->window->WindowResized(event.window.data1, event.window.data2);
			break;

		case SDL_MOUSEMOTION:
			if (event.motion.state & SDL_BUTTON_RMASK && App->scene->scene_window_is_hovered) 
			{
				float2 motion(event.motion.xrel, event.motion.yrel);
				App->cameras->scene_camera->RotateCameraWithMouseMotion(motion);
			}
			else if (event.motion.state & SDL_BUTTON_LMASK && App->scene->scene_window_is_hovered && App->cameras->IsOrbiting()) 
			{
				float2 motion(event.motion.xrel, event.motion.yrel);
				App->cameras->scene_camera->OrbitCameraWithMouseMotion(motion);
			}
			break;

		case SDL_MOUSEWHEEL:
			if (event.wheel.y > 0 && App->scene->scene_window_is_hovered)
			{
				App->cameras->scene_camera->MoveFoward();
			}
			else if (event.wheel.y < 0 && App->scene->scene_window_is_hovered)
			{
				App->cameras->scene_camera->MoveBackward();
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_RIGHT && App->scene->scene_window_is_hovered)
			{
				App->cameras->SetMovement(true);
			}
			if (event.button.button == SDL_BUTTON_LEFT && App->scene->scene_window_is_hovered)
			{
				float2 mouse_position = float2(event.button.x, event.button.y);
				App->scene->MousePicking(mouse_position);
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_RIGHT)
			{
				App->cameras->SetMovement(false);
			}
			break;

		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_LALT)
			{
				App->cameras->SetOrbit(true);
			}
			else if (event.key.keysym.sym == SDLK_LSHIFT)
			{
				App->cameras->scene_camera->SetSpeedUp(true);
			}
			else if (event.key.keysym.sym == SDLK_f)
			{
				if (App->scene->hierarchy.selected_game_object != nullptr)
				{
					App->cameras->scene_camera->Focus(App->scene->hierarchy.selected_game_object->aabb.bounding_box);
				}
			}
			break;

		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_LALT)
			{
				App->cameras->SetOrbit(false);
			}
			else if (event.key.keysym.sym == SDLK_LSHIFT)
			{
				App->cameras->scene_camera->SetSpeedUp(false);
			}
			break;

		case SDL_DROPFILE:
			char *dropped_filedir = event.drop.file;
			switch (App->filesystem->GetFileType(dropped_filedir))
			{
			case ModuleFileSystem::FileType::MODEL:
				App->ui->editor_ui->file_explorer_ui.CopyFileToSelectedFolder(dropped_filedir);
				//App->model_loader->LoadModel(dropped_filedir);
				break;
			default:
				break;
			}
			SDL_free(dropped_filedir);
			
			break;
		}
	}

	keyboard = SDL_GetKeyboardState(NULL);

	if (App->cameras->IsMovementEnabled())
	{
		if (keyboard[SDL_SCANCODE_Q]) 
		{
			App->cameras->scene_camera->MoveUp();
		}

		if (keyboard[SDL_SCANCODE_E])
		{
			App->cameras->scene_camera->MoveDown();
		}

		if (keyboard[SDL_SCANCODE_W])
		{
			App->cameras->scene_camera->MoveFoward();
		}

		if (keyboard[SDL_SCANCODE_S])
		{
			App->cameras->scene_camera->MoveBackward();
		}

		if (keyboard[SDL_SCANCODE_A])
		{
			App->cameras->scene_camera->MoveLeft();
		}

		if (keyboard[SDL_SCANCODE_D])
		{
			App->cameras->scene_camera->MoveRight();
		}
	}

	if (keyboard[SDL_SCANCODE_UP])
	{
		App->cameras->scene_camera->RotatePitch(-1.f);
	}

	if (keyboard[SDL_SCANCODE_DOWN])
	{
		App->cameras->scene_camera->RotatePitch(1.f);
	}

	if (keyboard[SDL_SCANCODE_LEFT])
	{
		App->cameras->scene_camera->RotateYaw(-1.f);
	}

	if (keyboard[SDL_SCANCODE_RIGHT])
	{
		App->cameras->scene_camera->RotateYaw(1.f);
	}
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	APP_LOG_INFO("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}


void ModuleInput::ShowInputOptions()
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
			if (ImGui::IsMousePosValid())
				ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
			else
				ImGui::Text("Mouse pos: <INVALID>");
			ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
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
	}
}