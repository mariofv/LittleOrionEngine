#include "ModuleCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "GameObject.h"
#include "Component/ComponentCamera.h"

#include "imgui.h"
#include <SDL/SDL.h>
#include <FontAwesome5/IconsFontAwesome5.h>

bool ModuleCamera::Init()
{
	APP_LOG_SECTION("************ Module Camera Init ************");
	int windowWidth, windowHeight;
	SDL_GetWindowSize(App->window->window, &windowWidth, &windowHeight);
	
	scene_camera_game_object = new GameObject();
	scene_camera_component = (ComponentCamera*)scene_camera_game_object->CreateComponent(Component::ComponentType::CAMERA);

	speed_up = 1;
	is_orbiting = false;

	return true;
}

// Called every draw update
update_status ModuleCamera::Update()
{
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleCamera::CleanUp()
{
	delete scene_camera_game_object;
	return true;
}

ComponentCamera* ModuleCamera::CreateComponentCamera() const
{
	return new ComponentCamera();
}

void ModuleCamera::SetOrbit(const bool is_orbiting)
{
	this->is_orbiting = is_orbiting;
}

bool ModuleCamera::IsOrbiting() const
{
	return is_orbiting;
}

void ModuleCamera::SetMovement(const bool movement_enabled)
{
	this->movement_enabled = movement_enabled;
}

bool ModuleCamera::IsMovementEnabled() const
{
	return movement_enabled;
}

void ModuleCamera::ShowCameraOptions()
{
	scene_camera_component->ShowComponentWindow();
}