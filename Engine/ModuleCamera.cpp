#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"

#include "imgui.h"
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
	speed_up = 1;
	is_orbiting = false;

	// CREATES PROJECTION MATRIX
	camera_frustum.type = FrustumType::PerspectiveFrustum;
	camera_frustum.pos = float3::unitX;
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
	float3x3 rotation_matrix = float3x3::LookAt(camera_frustum.front, orientation, camera_frustum.up, float3::unitY);
	camera_frustum.front = rotation_matrix * camera_frustum.front;
	camera_frustum.up = rotation_matrix * camera_frustum.up;

	generateMatrices();
}

void ModuleCamera::LookAt(const float3 focus)
{
	float3 look_direction = (focus - camera_frustum.pos).Normalized();
	SetOrientation(look_direction);

	generateMatrices();
}

void ModuleCamera::LookAt(const float x, const float y, const float z)
{
	LookAt(float3(x, y, z));
}

void ModuleCamera::Center(const BoundingBox *bounding_box)
{
	// Adapt size to bounding box
	float containing_sphere_radius = bounding_box->size.Length()/2;
	camera_frustum.farPlaneDistance = 25 * containing_sphere_radius;

	// TODO: Change magic numbers here. Also we are assuming that object is always centered in (0,0,0)!
	// Move position visualize bounding box
	camera_frustum.pos = float3::unitY * containing_sphere_radius/2 + float3::unitZ * -3 * containing_sphere_radius; 
	camera_frustum.up = float3::unitY;
	camera_frustum.front = float3::unitZ;

	generateMatrices();
}

void ModuleCamera::MoveUp()
{
	const float distance = CAMERA_MOVEMENT_SPEED * speed_up;
	float3 new_camera_pos = camera_frustum.pos;
	new_camera_pos.y = new_camera_pos.y + distance;
	camera_frustum.pos = new_camera_pos;

	generateMatrices();
}

void ModuleCamera::MoveDown()
{
	const float distance = CAMERA_MOVEMENT_SPEED * speed_up;
	float3 new_camera_pos = camera_frustum.pos;
	new_camera_pos.y = new_camera_pos.y - distance;
	camera_frustum.pos = new_camera_pos;

	generateMatrices();
}

void ModuleCamera::MoveFoward()
{
	const float distance = CAMERA_MOVEMENT_SPEED * speed_up;
	camera_frustum.pos += camera_frustum.front.ScaledToLength(distance);

	generateMatrices();
}

void ModuleCamera::MoveBackward()
{
	const float distance = CAMERA_MOVEMENT_SPEED * speed_up;
	camera_frustum.pos -= camera_frustum.front.ScaledToLength(distance);

	generateMatrices();
}

void ModuleCamera::MoveLeft()
{
	const float distance = CAMERA_MOVEMENT_SPEED * speed_up;
	camera_frustum.pos -= camera_frustum.WorldRight().ScaledToLength(distance);

	generateMatrices();
}

void ModuleCamera::MoveRight()
{
	const float distance = CAMERA_MOVEMENT_SPEED * speed_up;
	camera_frustum.pos = camera_frustum.pos + camera_frustum.WorldRight().ScaledToLength(distance);

	generateMatrices();
}

void ModuleCamera::MouseXMotion(const float x_motion)
{
	is_orbiting ? OrbitX(x_motion) : RotateYaw(x_motion);
}

void ModuleCamera::MouseYMotion(const float y_motion)
{
	is_orbiting ? OrbitY(y_motion) : RotatePitch(y_motion);
}

void ModuleCamera::RotatePitch(const float angle)
{
	const float adjusted_angle = angle * -CAMERA_ROTATION_SPEED;
	const float current_angle = asinf(camera_frustum.front.y / camera_frustum.front.Length());
	if (abs(current_angle + adjusted_angle) >= math::pi / 2) {
		return;
	}
	float3x3 rotation_matrix = float3x3::identity;
	rotation_matrix.SetRotatePart(camera_frustum.WorldRight(), adjusted_angle);
	camera_frustum.up = rotation_matrix * camera_frustum.up;
	camera_frustum.front = rotation_matrix * camera_frustum.front;

	generateMatrices();
}

void ModuleCamera::RotateYaw(const float angle)
{
	const float adjusted_angle = angle * -CAMERA_ROTATION_SPEED;
	float3x3 rotation_matrix = float3x3::RotateY(adjusted_angle);
	camera_frustum.up = rotation_matrix * camera_frustum.up;
	camera_frustum.front = rotation_matrix * camera_frustum.front;

	generateMatrices();
}

void ModuleCamera::OrbitX(const float angle)
{
	const float adjusted_angle = angle * -CAMERA_ROTATION_SPEED;
	float3x3 rotation_matrix = float3x3::RotateY(adjusted_angle);
	camera_frustum.up = rotation_matrix * camera_frustum.up;
	camera_frustum.front = rotation_matrix * camera_frustum.front;
	camera_frustum.pos = rotation_matrix * camera_frustum.pos;

	LookAt(float3::zero); // TODO: There must be a way to orbit around a focus point, ask this

	generateMatrices();
}

void ModuleCamera::OrbitY(const float angle)
{
	const float adjusted_angle = angle * -CAMERA_ROTATION_SPEED;
	const float current_angle = asinf(camera_frustum.front.y / camera_frustum.front.Length());
	if (abs(current_angle + adjusted_angle) >= math::pi / 2) {
		return;
	}

	float3x3 rotation_matrix = float3x3::identity;
	rotation_matrix.SetRotatePart(camera_frustum.WorldRight(), adjusted_angle);
	camera_frustum.up = rotation_matrix * camera_frustum.up;
	camera_frustum.front = rotation_matrix * camera_frustum.front;
	camera_frustum.pos = rotation_matrix * camera_frustum.pos;

	LookAt(float3::zero);

	generateMatrices();
}

void ModuleCamera::SetOrbit(const bool is_orbiting)
{
	this->is_orbiting = is_orbiting;
}

void ModuleCamera::SetSpeedUp(const bool is_speeding_up)
{
	speed_up = is_speeding_up ? SPEED_UP_FACTOR : 1;
}

void ModuleCamera::generateMatrices()
{
	proj = camera_frustum.ProjectionMatrix();
	view = camera_frustum.ViewMatrix();
}

void ModuleCamera::ShowCameraOptions()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::SliderFloat("FOV", &camera_frustum.horizontalFov, 0, 2 * 3.14f))
		{
			App->cameras->SetFOV(camera_frustum.horizontalFov);
		}

		if (ImGui::SliderFloat("Aspect Ratio", &aspect_ratio, 0, 10))
		{
			App->cameras->SetAspectRatio(aspect_ratio);
		}

		if (ImGui::SliderFloat("Near plane", &camera_frustum.nearPlaneDistance, 1, camera_frustum.farPlaneDistance + 1))
		{
			App->cameras->SetNearDistance(camera_frustum.nearPlaneDistance);
		}

		if (ImGui::SliderFloat("Far plane", &camera_frustum.farPlaneDistance, camera_frustum.nearPlaneDistance + 1, camera_frustum.nearPlaneDistance + 1000))
		{
			App->cameras->SetFarDistance(camera_frustum.farPlaneDistance);
		}
	}
}