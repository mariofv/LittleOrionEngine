#include "Main/Globals.h"
#include "Main/Application.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "Main/GameObject.h"
#include "Component/ComponentCamera.h"

#include <algorithm>
#include <SDL/SDL.h>

bool ModuleCamera::Init()
{
	APP_LOG_SECTION("************ Module Camera Init ************");
	int windowWidth, windowHeight;
	SDL_GetWindowSize(App->window->window, &windowWidth, &windowHeight);

	scene_camera_game_object = new GameObject();
	scene_camera_game_object->transform.SetTranslation(float3(0.5f, 2.f, -15.f));
	scene_camera = (ComponentCamera*)scene_camera_game_object->CreateComponent(Component::ComponentType::CAMERA);
	scene_camera->SetFarDistance(2000);
	scene_camera->depth = -1;
	
	skybox = new Skybox();
	scene_camera->SetClearMode(ComponentCamera::ClearMode::SKYBOX);

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	HandleSceneCameraMovements();
	return update_status::UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleCamera::Update()
{
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
	auto it = std::find(cameras.begin(), cameras.end(), camera_to_remove);
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

void ModuleCamera::SetOrbit(bool is_orbiting)
{
	this->is_orbiting = is_orbiting;
}

bool ModuleCamera::IsOrbiting() const
{
	return is_orbiting;
}

void ModuleCamera::SetMovement(bool movement_enabled)
{
	this->movement_enabled = movement_enabled;
}

void ModuleCamera::HandleSceneCameraMovements()
{
	// Mouse wheel
	if (App->input->GetMouseWheelMotion() > 0 && App->editor->scene_panel->IsHovered())
	{
		scene_camera->MoveFoward();
	}
	else if (App->input->GetMouseWheelMotion() < 0 && App->editor->scene_panel->IsHovered())
	{
		scene_camera->MoveBackward();
	}

	// Mouse motion
	if (App->input->IsMouseMoving() && App->editor->scene_panel->IsHovered())
	{
		float2 motion = App->input->GetMouseMotion();
		scene_camera->RotateCameraWithMouseMotion(motion);
	}
	else if (App->input->IsMouseMoving() && App->editor->scene_panel->IsHovered() && IsOrbiting())
	{
		float2 motion = App->input->GetMouseMotion();
		if (App->editor->selected_game_object != nullptr)
		{
			scene_camera->OrbitCameraWithMouseMotion(motion, App->editor->selected_game_object->transform.GetGlobalTranslation());
		}
		else
		{
			scene_camera->RotateCameraWithMouseMotion(motion);
		}
	}

	// Mouse button down
	if (App->input->GetMouseButtonDown(MouseButton::Right) && App->editor->scene_panel->IsHovered())
	{
		SetMovement(true);
	}
	if (App->input->GetMouseButtonDown(MouseButton::Left) && App->editor->scene_panel->IsHovered() && !IsOrbiting())
	{
		float2 position = App->input->GetMousePosition();
		App->editor->scene_panel->MousePicking(position);

		if (App->input->GetMouseClicks() == 2 && App->editor->selected_game_object != nullptr)
		{
			scene_camera->Center(App->editor->selected_game_object->aabb.global_bounding_box);
		}
	}

	// Mouse button up
	if (App->input->GetMouseButtonUp(MouseButton::Right))
	{
		SetMovement(false);
	}

	// Key down
	if (App->input->GetKeyDown(KeyCode::LeftAlt))
	{
		SetOrbit(true);
	}
	else if (App->input->GetKeyDown(KeyCode::LeftShift))
	{
		scene_camera->SetSpeedUp(true);
	}
	else if (App->input->GetKeyDown(KeyCode::F))
	{
		if (App->editor->selected_game_object != nullptr)
		{
			App->cameras->scene_camera->Center(App->editor->selected_game_object->aabb.global_bounding_box);
		}
	}

	// Key up
	if (App->input->GetKeyUp(KeyCode::LeftAlt))
	{
		SetOrbit(false);
	}
	else if (App->input->GetKeyUp(KeyCode::LeftShift))
	{
		scene_camera->SetSpeedUp(false);
	}

	// Key hold
	if (IsMovementEnabled())
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
			scene_camera->MoveFoward();
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

bool ModuleCamera::IsMovementEnabled() const
{
	return movement_enabled;
}