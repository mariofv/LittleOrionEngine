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

	SetDirectionalLightFrustums();
	SetMainCameraFrustums();

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
	BROFILER_CATEGORY("Scene Camera Update", Profiler::Color::Lavender);
	SelectMainCamera();
	scene_camera->Update();
	UpdateMainCameraFrustums();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate()
{

	return update_status::UPDATE_CONTINUE;
}

void ModuleCamera::SetDirectionalLightFrustums()
{
	dir_light_game_object = App->scene->CreateGameObject();
	dir_light_game_object->transform.SetTranslation(float3(0, 0, 0));
	dir_light_game_object_mid = App->scene->CreateGameObject();
	dir_light_game_object_mid->transform.SetTranslation(float3(0, 0, 0));
	dir_light_game_object_far = App->scene->CreateGameObject();
	dir_light_game_object_far->transform.SetTranslation(float3(0, 0, 0));
	


	directional_light_camera = (ComponentCamera*)dir_light_game_object->CreateComponent(Component::ComponentType::CAMERA);
	directional_light_camera->depth = -1;
	directional_light_camera->SetClearMode(ComponentCamera::ClearMode::SKYBOX);
	directional_light_camera->owner = dir_light_game_object;
	directional_light_camera->camera_frustum.type = FrustumType::OrthographicFrustum;


	directional_light_mid = (ComponentCamera*)dir_light_game_object_mid->CreateComponent(Component::ComponentType::CAMERA);
	directional_light_mid->depth = -1;
	directional_light_mid->SetClearMode(ComponentCamera::ClearMode::SKYBOX);
	directional_light_mid->owner = dir_light_game_object;
	directional_light_mid->camera_frustum.type = FrustumType::OrthographicFrustum;


	directional_light_far = (ComponentCamera*)dir_light_game_object_far->CreateComponent(Component::ComponentType::CAMERA);
	directional_light_far->depth = -1;
	directional_light_far->SetClearMode(ComponentCamera::ClearMode::SKYBOX);
	directional_light_far->owner = dir_light_game_object;
	directional_light_far->camera_frustum.type = FrustumType::OrthographicFrustum;


	App->cameras->directional_light_camera->SetNearDistance(0);
	App->cameras->directional_light_mid->SetNearDistance(0);
	App->cameras->directional_light_far->SetNearDistance(0);


	

}
void ModuleCamera::UpdateDirectionalLightFrustums(float3 max, float3 min)
{
	//Setting far planes also from AABB 
	App->cameras->directional_light_camera->SetFarDistance(max.z - min.z);
	App->cameras->directional_light_mid->SetFarDistance(max.z - min.z);
	App->cameras->directional_light_far->SetFarDistance(max.z - min.z);

	//Setting frustums' width and height

	App->cameras->directional_light_camera->camera_frustum.orthographicWidth = max.x - min.x;
	App->cameras->directional_light_camera->camera_frustum.orthographicHeight = max.y - min.y;

	App->cameras->directional_light_mid->camera_frustum.orthographicWidth = max.x - min.x;
	App->cameras->directional_light_mid->camera_frustum.orthographicHeight = max.y - min.y;

	App->cameras->directional_light_far->camera_frustum.orthographicWidth = max.x - min.x;
	App->cameras->directional_light_far->camera_frustum.orthographicHeight = max.y - min.y;
	
	directional_light_camera->Update();
	directional_light_mid->Update();
	directional_light_far->Update();
}

void ModuleCamera::SetMainCameraFrustums()
{
	main_close = App->scene->CreateGameObject();
	main_close->transform.SetTranslation(float3(0, 0, 0));

	main_mid = App->scene->CreateGameObject();
	main_mid->transform.SetTranslation(float3(0, 0, 0));

	main_far = App->scene->CreateGameObject();
	main_far->transform.SetTranslation(float3(0, 0, 0));

	camera_close = (ComponentCamera*)main_close->CreateComponent(Component::ComponentType::CAMERA);
	camera_close->depth = -1;
	camera_close->SetClearMode(ComponentCamera::ClearMode::SKYBOX);
	camera_close->camera_frustum.type = FrustumType::PerspectiveFrustum;


	camera_mid = (ComponentCamera*)main_mid->CreateComponent(Component::ComponentType::CAMERA);
	camera_mid->depth = -1;
	camera_mid->SetClearMode(ComponentCamera::ClearMode::SKYBOX);
	camera_mid->owner = main_close;
	camera_mid->camera_frustum.type = FrustumType::PerspectiveFrustum;


	camera_far = (ComponentCamera*)main_far->CreateComponent(Component::ComponentType::CAMERA);
	camera_far->depth = -1;
	camera_far->SetClearMode(ComponentCamera::ClearMode::SKYBOX);
	camera_far->owner = main_close;
	camera_far->camera_frustum.type = FrustumType::PerspectiveFrustum;

	camera_close->SetNearDistance(1);
	camera_mid->SetNearDistance(1);
	camera_far->SetNearDistance(1);
}
void ModuleCamera::UpdateMainCameraFrustums()
{

	if (main_camera != nullptr)
	{

		camera_close->SetFarDistance(main_camera->camera_frustum.farPlaneDistance / 3);
		camera_mid->SetFarDistance(main_camera->camera_frustum.farPlaneDistance * 2 / 3);
		camera_far->SetFarDistance(main_camera->camera_frustum.farPlaneDistance);

		camera_close->SetAspectRatio(main_camera->camera_frustum.AspectRatio());
		camera_mid->SetAspectRatio(main_camera->camera_frustum.AspectRatio());
		camera_far->SetAspectRatio(main_camera->camera_frustum.AspectRatio());

		main_close->transform = main_camera->owner->transform; //All main camera frustums are atached to the main_close game object

	}

	camera_close->Update();
	camera_mid->Update();
	camera_far->Update();

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