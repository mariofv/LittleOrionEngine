#include "ModuleScene.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"

void ModuleScene::ShowSceneWindow() const
{
	if (ImGui::Begin(ICON_FA_TH " Scene"))
	{
		float imgui_window_width = ImGui::GetWindowWidth();
		float imgui_window_height = ImGui::GetWindowHeight();
		App->cameras->SetAspectRatio(imgui_window_width / imgui_window_height);
		App->renderer->GenerateFrameTexture(imgui_window_width, imgui_window_height);

		ImGui::GetWindowDrawList()->AddImage(
			(void *)App->renderer->frame_texture,
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(
				ImGui::GetCursorScreenPos().x + imgui_window_width,
				ImGui::GetCursorScreenPos().y + imgui_window_height
			),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
	}
	ImGui::End();
}