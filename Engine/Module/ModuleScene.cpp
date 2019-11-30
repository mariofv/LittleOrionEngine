#include "ModuleScene.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"

#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>


bool ModuleScene::Init()
{
	root = new GameObject();
	
	return true;
}

bool ModuleScene::CleanUp()
{
	delete root;

	return true;
}

GameObject* ModuleScene::CreateGameObject()
{
	std::string created_game_object_name = hierarchy.GetNextGameObjectName();
	GameObject *created_game_object = root->CreateChild(created_game_object_name);
	
	return created_game_object;
}

void ModuleScene::Render() const
{
	root->Update();
	root->Render();
}

GameObject* ModuleScene::GetRoot() const
{
	return root;
}

void ModuleScene::ShowSceneWindow()
{
	if (ImGui::Begin(ICON_FA_TH " Scene"))
	{
		scene_window_is_hovered = ImGui::IsWindowHovered();
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
	if (App->cameras->MovementEnabled() && scene_window_is_hovered)
	{
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
	}
	ImGui::End();
}