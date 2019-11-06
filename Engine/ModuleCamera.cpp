#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "SDL.h"

ModuleCamera::ModuleCamera()
{
}

// Destructor
ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	int windowWidth, windowHeight;
	SDL_GetWindowSize(App->window->window, &windowWidth, &windowHeight);
	
	aspect_ratio = (float)windowWidth / windowHeight;

	// CREATES PROJECTION MATRIX
	camera_frustum.type = FrustumType::PerspectiveFrustum;
	camera_frustum.pos = float3::zero;
	camera_frustum.front = -float3::unitZ;
	camera_frustum.up = float3::unitY;
	camera_frustum.nearPlaneDistance = 1.f;
	camera_frustum.farPlaneDistance = 100.0f;
	camera_frustum.verticalFov = math::pi / 4.0f;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);

	generateMatrices();

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	return UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleCamera::Update()
{

	return UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate()
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleCamera::CleanUp()
{
	return true;
}

void ModuleCamera::SetFOV(const float fov)
{
	camera_frustum.verticalFov = fov;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);

	generateMatrices();
}

void ModuleCamera::SetAspectRatio(const float aspect_ratio)
{
	this->aspect_ratio = aspect_ratio;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);

	generateMatrices();
}

void ModuleCamera::SetNearDistance(const float distance)
{
	camera_frustum.nearPlaneDistance = distance;

	generateMatrices();
}

void ModuleCamera::SetFarDistance(const float distance)
{
	camera_frustum.farPlaneDistance = distance;

	generateMatrices();
}

void ModuleCamera::SetPosition(const float3 position)
{
	camera_frustum.pos = position;

	generateMatrices();
}

void ModuleCamera::SetOrientation(const float3 orientation)
{

	generateMatrices();
}

void ModuleCamera::LookAt(const float x, const float y, const float z)
{
	generateMatrices();

}

void ModuleCamera::generateMatrices()
{
	proj = camera_frustum.ProjectionMatrix();
	view = camera_frustum.ViewMatrix();
}