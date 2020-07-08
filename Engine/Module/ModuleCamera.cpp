#include "ModuleCamera.h"

#include "Component/ComponentCamera.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Main/Globals.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleResourceManager.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"

#include "ResourceManagement/ResourcesDB/CoreResources.h"

#include <algorithm>
#include <Brofiler/Brofiler.h>
#include <SDL/SDL.h>

bool ModuleCamera::Init()
{
	APP_LOG_SECTION("************ Module Camera Init ************");
	int windowWidth, windowHeight;
	SDL_GetWindowSize(App->window->window, &windowWidth, &windowHeight);

	scene_camera_game_object = new GameObject();
	scene_camera_game_object->transform.SetTranslation(float3(0.f, 2.f, 15.f));
	scene_camera_game_object->transform.SetRotation(float3(0.f, 180.f, 0.f));
	scene_camera = (ComponentCamera*)scene_camera_game_object->CreateComponent(Component::ComponentType::CAMERA);
	scene_camera->SetFarDistance(5000);
	scene_camera->depth = -1;
	scene_camera->SetClearMode(ComponentCamera::ClearMode::SKYBOX);
	world_skybox = App->resources->Load<Skybox>((uint32_t)CoreResource::DEFAULT_SKYBOX);

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	BROFILER_CATEGORY("Module Camera PreUpdate", Profiler::Color::OldLace);
	HandleSceneCameraMovements();
	return update_status::UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleCamera::Update()
{
	BROFILER_CATEGORY("Module Camera Update", Profiler::Color::OliveDrab);
	SelectMainCamera();
	scene_camera->Update();
	return update_status::UPDATE_CONTINUE;
}



bool ModuleCamera::CleanUp()
{
	for (auto& camera : cameras)
	{
		camera->owner->RemoveComponent(camera);
	}
	cameras.clear();
	main_camera = nullptr;

	delete scene_camera_game_object;

	return true;
}

ComponentCamera* ModuleCamera::CreateComponentCamera()
{
	ComponentCamera * new_camera = new ComponentCamera();
	cameras.push_back(new_camera);
	return new_camera;
}


void ModuleCamera::RemoveComponentCamera(ComponentCamera* camera_to_remove)
{
	const auto it = std::find(cameras.begin(), cameras.end(), camera_to_remove);
	if (*it == main_camera)
	{
		main_camera = nullptr;
	}
	if (it != cameras.end())
	{
		delete *it;
		cameras.erase(it);
	}
}

void ModuleCamera::SelectMainCamera()
{
	main_camera = nullptr;

	for (auto& camera : cameras)
	{
		if (camera->IsEnabled() && camera != scene_camera)
		{
			if (main_camera == nullptr)
			{
				main_camera = camera;
			}
			else if (main_camera->depth < camera->depth)
			{
				main_camera = camera;
			}
		}
	}
}

bool ModuleCamera::IsSceneCameraOrbiting() const
{
	return is_orbiting;
}

void ModuleCamera::SetMovement(bool movement_enabled)
{
	this->movement_enabled = movement_enabled;
}

bool ModuleCamera::IsSceneCameraMoving() const
{
	return movement_enabled;
}

void ModuleCamera::HandleSceneCameraMovements()
{
	if (!(App->editor->scene_panel->IsHovered()))
	{
		return;
	}

	// Mouse wheel
	if (App->input->GetMouseWheelMotion() > 0)
	{
		scene_camera->MoveForward();
	}
	else if (App->input->GetMouseWheelMotion() < 0)
	{
		scene_camera->MoveBackward();
	}

	// Mouse motion
	if (App->input->IsMouseMoving())
	{
		float2 motion = App->input->GetMouseMotion();

		if (IsSceneCameraMoving() && !IsSceneCameraOrbiting())
		{
			scene_camera->RotateCameraWithMouseMotion(motion);
		}
		else if (!IsSceneCameraMoving() && IsSceneCameraOrbiting() && orbit_movement_enabled)
		{
			if (App->editor->selected_game_object != nullptr)
			{
				scene_camera->OrbitCameraWithMouseMotion(motion, App->editor->selected_game_object->transform.GetGlobalTranslation());
			}
			else
			{
				scene_camera->RotateCameraWithMouseMotion(motion);
			}
		}
	}

	// Mouse button down
	if (App->input->GetMouseButtonDown(MouseButton::Right))
	{
		SetMovement(true);
	}

	// Mouse button down
	if (App->input->GetMouseButtonDown(MouseButton::Left))
	{
		orbit_movement_enabled = true;
	}

	if (App->input->GetMouseButtonDown(MouseButton::Left) && !IsSceneCameraOrbiting())
	{
		float2 position = App->input->GetMousePosition();
		App->editor->scene_panel->MousePicking(position);

		if (App->input->GetMouseClicks() == 2 && App->editor->selected_game_object != nullptr)
		{
			scene_camera->Center(App->editor->selected_game_object->aabb.global_bounding_box);
		}
	}

	// Key down
	if (App->input->GetKeyDown(KeyCode::LeftAlt))
	{
		is_orbiting = true;
	}

	if (App->input->GetKeyDown(KeyCode::LeftShift))
	{
		scene_camera->SetSpeedUp(true);
	}

	if (App->input->GetKeyDown(KeyCode::F))
	{
		if (App->editor->selected_game_object != nullptr)
		{
			App->cameras->scene_camera->Center(App->editor->selected_game_object->aabb.global_bounding_box);
		}
	}

	// Mouse button up
	if (App->input->GetMouseButtonUp(MouseButton::Right))
	{
		SetMovement(false);
	}

	// Mouse button up
	if (App->input->GetMouseButtonUp(MouseButton::Left))
	{
		orbit_movement_enabled = false;
	}

	// Key up
	if (App->input->GetKeyUp(KeyCode::LeftAlt))
	{
		is_orbiting = false;
	}

	if (App->input->GetKeyUp(KeyCode::LeftShift))
	{
		scene_camera->SetSpeedUp(false);
	}

	// Key hold
	if (IsSceneCameraMoving())
	{
		if (App->input->GetKey(KeyCode::Q))
		{
			scene_camera->MoveUp();
		}
		if (App->input->GetKey(KeyCode::E))
		{
			scene_camera->MoveDown();
		}
		if (App->input->GetKey(KeyCode::W))
		{
			scene_camera->MoveForward();
		}
		if (App->input->GetKey(KeyCode::S))
		{
			scene_camera->MoveBackward();
		}
		if (App->input->GetKey(KeyCode::A))
		{
			scene_camera->MoveLeft();
		}
		if (App->input->GetKey(KeyCode::D))
		{
			scene_camera->MoveRight();
		}
	}
	if (App->input->GetKey(KeyCode::UpArrow))
	{
		scene_camera->RotatePitch(-1.f);
	}
	if (App->input->GetKey(KeyCode::DownArrow))
	{
		scene_camera->RotatePitch(1.f);
	}
	if (App->input->GetKey(KeyCode::LeftArrow))
	{
		scene_camera->RotateYaw(-1.f);
	}
	if (App->input->GetKey(KeyCode::RightArrow))
	{
		scene_camera->RotateYaw(1.f);
	}
}