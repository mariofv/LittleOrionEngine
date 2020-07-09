#include "PanelGame.h"

#include "Component/ComponentCamera.h"
#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleLight.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleRender.h"

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

			game_window_content_area_width = game_window_content_area_max_point.x - game_window_content_area_pos.x;
			game_window_content_area_height = game_window_content_area_max_point.y - game_window_content_area_pos.y;

			
			App->lights->RecordShadowsFrameBuffers((GLsizei)game_window_content_area_width, (GLsizei)game_window_content_area_height);
			App->cameras->main_camera->RecordFrame((GLsizei)game_window_content_area_width, (GLsizei)game_window_content_area_height);
			App->cameras->main_camera->RecordDebugDraws();


			ImGui::Image(
				(void *)App->cameras->main_camera->GetLastRecordedFrame(),
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