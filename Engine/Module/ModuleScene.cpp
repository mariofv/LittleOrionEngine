#include "ModuleScene.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "Component/ComponentCamera.h"
#include "Config.h"

#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>
#include <algorithm>

bool ModuleScene::Init()
{
	root = new GameObject();
	GameObject * camera = CreateGameObject();
	camera->name = "Main Camera";
	ComponentCamera * component_camera = static_cast<ComponentCamera*>(camera->CreateComponent(Component::ComponentType::CAMERA));
	App->cameras->active_camera = component_camera;
	App->cameras->active_camera->SetPosition(float3(0.f, 1.f, -10.f));
	App->cameras->active_camera->SetFarDistance(500);
	App->renderer->GenerateQuadTree(); // TODO: Move this to load scene and save scene
	return true;
}

update_status ModuleScene::Update()
{
	root->Update();
	return update_status::UPDATE_CONTINUE;
}


bool ModuleScene::CleanUp()
{
	delete root;

	return true;
}

GameObject* ModuleScene::CreateGameObject()
{
	std::string created_game_object_name = hierarchy.GetNextGameObjectName();
	std::unique_ptr<GameObject> created_game_object = std::make_unique<GameObject>(created_game_object_name);
	created_game_object->SetParent(root);

	GameObject * created_game_object_ptr = created_game_object.get();
	game_objects_ownership.emplace_back(std::move(created_game_object));
	return created_game_object_ptr;
}

GameObject* ModuleScene::CreateChildGameObject(GameObject *parent)
{
	GameObject * created_game_object_ptr = CreateGameObject();
	parent->AddChild(created_game_object_ptr);

	return created_game_object_ptr;
}

void ModuleScene::RemoveGameObject(GameObject * game_object_to_remove)
{
	auto it = std::find_if(game_objects_ownership.begin(), game_objects_ownership.end(), [game_object_to_remove](auto const & game_object) 
	{
		return game_object_to_remove == game_object.get();
	});
	if (it != game_objects_ownership.end())
	{
		game_objects_ownership.erase(it);
	}
}

GameObject* ModuleScene::GetRoot() const
{
	return root;
}

GameObject* ModuleScene::GetGameObject(unsigned int UUID) const
{
	for (auto& game_object : game_objects_ownership)
	{
		if (game_object->UUID == UUID) 
		{
			return game_object.get();
		}
	}

	return nullptr;
}

void ModuleScene::Save(SerializedScene& serialized_scene) const
{
	std::vector<Config*> game_objects_config;
	for (auto &game_object : game_objects_ownership)
	{
		Config* game_object_config = new Config();
		game_object_config->SetAllocator(serialized_scene.GetSceneAllocator());

		game_object->Save(*game_object_config);
		game_objects_config.push_back(game_object_config);
	}
	serialized_scene.AddGameObjectsConfig(game_objects_config);
}

void ModuleScene::Load(const SerializedScene& serialized_scene)
{
	std::vector<Config*> game_objects_config;
	serialized_scene.GetGameObjectsConfig(game_objects_config);
}

void ModuleScene::ShowFrameBufferTab(ComponentCamera & camera_frame_buffer_to_show, const char * title)
{
	if (ImGui::BeginTabItem(title))
	{
		scene_window_is_hovered = ImGui::IsWindowHovered(); // TODO: This should be something like ImGui::IsTabHovered (such function doesn't exist though)

		float imgui_window_width = ImGui::GetWindowWidth(); 
		float imgui_window_height = ImGui::GetWindowHeight();
		camera_frame_buffer_to_show.RecordFrame(imgui_window_width, imgui_window_height);

		ImGui::GetWindowDrawList()->AddImage(
			(void *)camera_frame_buffer_to_show.GetLastRecordedFrame(),
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(
				ImGui::GetCursorScreenPos().x + imgui_window_width,
				ImGui::GetCursorScreenPos().y + imgui_window_height
			),
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
		if (App->cameras->IsMovementEnabled() && scene_window_is_hovered) // CHANGES CURSOR IF SCENE CAMERA MOVEMENT IS ENABLED
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
		}
		ImGui::EndTabItem();
	}

}