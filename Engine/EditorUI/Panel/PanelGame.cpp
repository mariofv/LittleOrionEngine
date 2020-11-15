#include "PanelGame.h"

#include "Component/ComponentCamera.h"
#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleInput.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleRender.h"

#include "Rendering/Viewport.h"

#include <Brofiler/Brofiler.h>
#include <imgui.h>
#include <FontAwesome5/IconsFontAwesome5.h>

PanelGame::PanelGame()
{
	opened = true;
	enabled = true;
	window_name = ICON_FA_GHOST " Game";
}

void PanelGame::Render()
{
	BROFILER_CATEGORY("Render Game Panel", Profiler::Color::BlueViolet);

	if (ImGui::Begin(ICON_FA_GHOST " Game", &opened))
	{
		hovered = ImGui::IsWindowHovered();
		focused = ImGui::IsWindowFocused();

		if(App->input->GetKey(KeyCode::LeftControl) &&
			App->input->GetKey(KeyCode::LeftShift) &&
			App->input->GetKeyDown(KeyCode::F))
		{
			fullscreen = !fullscreen;

			if(fullscreen)
			{
				previous_game_window_content_area_width = ImGui::GetWindowWidth();
				previous_game_window_content_area_height = ImGui::GetWindowHeight();
				game_window_content_area_width = 1920;
				game_window_content_area_height = 1080;			
			}
			else
			{
				game_window_content_area_width = previous_game_window_content_area_width;
				game_window_content_area_height = previous_game_window_content_area_height;
			}
			
			ImGui::SetWindowSize(ImVec2(game_window_content_area_width, game_window_content_area_height));
		}


		if (App->cameras->main_camera != nullptr)
		{
			ImVec2 game_window_pos_ImVec2 = ImGui::GetWindowPos();
			float2 game_window_pos = float2(game_window_pos_ImVec2.x, game_window_pos_ImVec2.y);

			ImVec2 game_window_content_area_max_point_ImVec2 = ImGui::GetWindowContentRegionMax();
			game_window_content_area_max_point_ImVec2 = ImVec2(
				game_window_content_area_max_point_ImVec2.x + game_window_pos_ImVec2.x,
				game_window_content_area_max_point_ImVec2.y + game_window_pos_ImVec2.y
			); // Pass from window space to screen space
			float2 game_window_content_area_max_point = float2(game_window_content_area_max_point_ImVec2.x, game_window_content_area_max_point_ImVec2.y);

			ImVec2 game_window_content_area_pos_ImVec2 = ImGui::GetCursorScreenPos();
			game_window_content_area_pos = float2(game_window_content_area_pos_ImVec2.x, game_window_content_area_pos_ImVec2.y);

			if(!fullscreen)
			{
				game_window_content_area_width = game_window_content_area_max_point.x - game_window_content_area_pos.x;
				game_window_content_area_height = game_window_content_area_max_point.y - game_window_content_area_pos.y;
			}

			App->renderer->game_viewport->SetSize(game_window_content_area_width, game_window_content_area_height);
			App->renderer->game_viewport->Render(App->cameras->main_camera);

			ImGui::Image(
				(void *)App->renderer->game_viewport->displayed_texture,
				ImVec2(game_window_content_area_width, game_window_content_area_height),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);
		}
		else
		{
			ShowEmptyGameWindow();
		}
	}
	ImGui::End();
}


void PanelGame::ShowEmptyGameWindow() const
{
	float window_width = ImGui::GetWindowWidth();
	ImGui::Dummy(ImVec2(0, ImGui::GetWindowHeight()*0.40f));
	ImGui::SetCursorPosX(window_width*0.45f);
	ImGui::Text("Display 1");
	ImGui::SetCursorPosX(window_width*0.40f);
	ImGui::Text("No Cameras Rendering");
}