#include "ModuleScene.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"

#include "imgui.h"
#include "IconsFontAwesome5.h"


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
	std::string created_game_object_name = GetNextGameObjectName();
	GameObject *created_game_object = new GameObject(created_game_object_name);
	created_game_object->depth_in_hierarchy = 1;
	root->AddChild(created_game_object);

	return created_game_object;
}

void ModuleScene::Render() const
{
	root->Update();
	root->Render();
}

std::string ModuleScene::GetNextGameObjectName()
{
	char tmp_string[64];
	sprintf_s(tmp_string, "GameObject (%d)", num_game_objects++);

	return std::string(tmp_string);
}

void ModuleScene::ShowHierarchyWindow()
{
	if (ImGui::Begin(ICON_FA_SITEMAP " Hierarchy"))
	{
		for (unsigned int i = 0; i < root->children.size(); ++i)
		{
			ImGui::PushID(i);
			root->children[i]->ShowGameObjectHierarchy();
			ImGui::PopID();
		}
		
		ImGui::BeginChild("Hierarchy###"); // This children window is used to create a clickable area for creating game objects in the root node
		ImGui::EndChild();
		if (ImGui::BeginPopupContextItem("Hierarchy###"))
		{
			if (ImGui::Selectable("Create GameObject"))
			{
				CreateGameObject();
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();

	
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