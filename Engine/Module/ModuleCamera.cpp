#include "ModuleCamera.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"

#include "imgui.h"
#include "SDL.h"
#include "IconsFontAwesome5.h"

bool ModuleCamera::Init()
{
	APP_LOG_SECTION("************ Module Camera Init ************");
	int windowWidth, windowHeight;
	SDL_GetWindowSize(App->window->window, &windowWidth, &windowHeight);
	
	aspect_ratio = (float)windowWidth / windowHeight;
	speed_up = 1;
	is_orbiting = false;

	camera_frustum.type = FrustumType::PerspectiveFrustum;
	camera_frustum.pos = float3::unitX;
	camera_frustum.front = -float3::unitZ;
	camera_frustum.up = float3::unitY;
	camera_frustum.nearPlaneDistance = 1.f;
	camera_frustum.farPlaneDistance = 100.0f;
	camera_frustum.verticalFov = math::pi / 4.0f;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);

	GenerateMatrices();

	return true;
}


// Called every draw update
update_status ModuleCamera::Update()
{
	if (is_focusing)
	{
		float3 zooming_direction = desired_focus_position - camera_frustum.pos;
		float distance_to_desired_zooming_position = zooming_direction.Length();
		float frame_focusing_distance = App->time->real_time_delta_time * camera_zooming_speed;
		if (distance_to_desired_zooming_position - frame_focusing_distance < 0) 
		{
			camera_frustum.pos = desired_focus_position;
			is_focusing = false;
		}
		else 
		{
			camera_frustum.pos += zooming_direction.ScaledToLength(frame_focusing_distance);
		}
		GenerateMatrices();
	}
	return update_status::UPDATE_CONTINUE;
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

	GenerateMatrices();
}

void ModuleCamera::SetAspectRatio(const float aspect_ratio)
{
	this->aspect_ratio = aspect_ratio;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);

	GenerateMatrices();
}

void ModuleCamera::SetNearDistance(const float distance)
{
	camera_frustum.nearPlaneDistance = distance;

	GenerateMatrices();
}

void ModuleCamera::SetFarDistance(const float distance)
{
	camera_frustum.farPlaneDistance = distance;

	GenerateMatrices();
}

void ModuleCamera::SetPosition(const float3 position)
{
	camera_frustum.pos = position;

	GenerateMatrices();
}

void ModuleCamera::SetOrientation(const float3 orientation)
{
	float3x3 rotation_matrix = float3x3::LookAt(camera_frustum.front, orientation, camera_frustum.up, float3::unitY);
	camera_frustum.front = rotation_matrix * camera_frustum.front;
	camera_frustum.up = rotation_matrix * camera_frustum.up;

	GenerateMatrices();
}

void ModuleCamera::LookAt(const float3 focus)
{
	float3 look_direction = (focus - camera_frustum.pos).Normalized();
	SetOrientation(look_direction);

	GenerateMatrices();
}

void ModuleCamera::LookAt(const float x, const float y, const float z)
{
	LookAt(float3(x, y, z));
}

void ModuleCamera::Center(const AABB &bounding_box)
{
	camera_frustum.up = float3::unitY;
	camera_frustum.front = float3::unitZ;

	float containing_sphere_radius = bounding_box.Size().Length()/2;

	// Adapt camera speed to bounding box size
	camera_movement_speed = CAMERA_MOVEMENT_SPEED_BOUNDING_BOX_RADIUS_FACTOR * containing_sphere_radius;
	camera_zooming_speed = CAMERA_ZOOMING_SPEED_BOUNDING_BOX_RADIUS_FACTOR * containing_sphere_radius;

	// Adapt far plane to visualize the whole bounding box
	camera_frustum.farPlaneDistance = FAR_PLANE_FACTOR * containing_sphere_radius;

	// Move camera position to visualize the whole bounding box
	camera_frustum.pos = bounding_box.CenterPoint() - camera_frustum.front * BOUNDING_BOX_DISTANCE_FACTOR * containing_sphere_radius;
	camera_frustum.pos.y = INITIAL_HEIGHT_FACTOR * containing_sphere_radius;

	GenerateMatrices();
}

void ModuleCamera::Focus(const AABB &bounding_box)
{
	LookAt(bounding_box.CenterPoint());

	float containing_sphere_radius = bounding_box.Size().Length() / 2;
	is_focusing = true;
	desired_focus_position = bounding_box.CenterPoint() - BOUNDING_BOX_DISTANCE_FACTOR * containing_sphere_radius * camera_frustum.front;

	GenerateMatrices();
}


void ModuleCamera::MoveUp()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	float3 new_camera_pos = camera_frustum.pos;
	new_camera_pos.y = new_camera_pos.y + distance;
	camera_frustum.pos = new_camera_pos;

	GenerateMatrices();
}

void ModuleCamera::MoveDown()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	float3 new_camera_pos = camera_frustum.pos;
	new_camera_pos.y = new_camera_pos.y - distance;
	camera_frustum.pos = new_camera_pos;

	GenerateMatrices();
}

void ModuleCamera::MoveFoward()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	camera_frustum.pos += camera_frustum.front.ScaledToLength(distance);

	GenerateMatrices();
}

void ModuleCamera::MoveBackward()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	camera_frustum.pos -= camera_frustum.front.ScaledToLength(distance);

	GenerateMatrices();
}

void ModuleCamera::MoveLeft()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	camera_frustum.pos -= camera_frustum.WorldRight().ScaledToLength(distance);

	GenerateMatrices();
}

void ModuleCamera::MoveRight()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	camera_frustum.pos = camera_frustum.pos + camera_frustum.WorldRight().ScaledToLength(distance);

	GenerateMatrices();
}

void ModuleCamera::RotatePitch(const float angle)
{
	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	const float current_angle = asinf(camera_frustum.front.y / camera_frustum.front.Length());
	if (abs(current_angle + adjusted_angle) >= math::pi / 2) {
		return;
	}
	float3x3 rotation_matrix = float3x3::identity;
	rotation_matrix.SetRotatePart(camera_frustum.WorldRight(), adjusted_angle);
	camera_frustum.up = rotation_matrix * camera_frustum.up;
	camera_frustum.front = rotation_matrix * camera_frustum.front;

	GenerateMatrices();
}

void ModuleCamera::RotateYaw(const float angle)
{
	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	float3x3 rotation_matrix = float3x3::RotateY(adjusted_angle);
	camera_frustum.up = rotation_matrix * camera_frustum.up;
	camera_frustum.front = rotation_matrix * camera_frustum.front;

	GenerateMatrices();
}

void ModuleCamera::OrbitX(const float angle)
{
	if (!is_orbiting)
	{
		return;
	}

	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	float3x3 rotation_matrix = float3x3::RotateY(adjusted_angle);
	camera_frustum.pos = rotation_matrix * camera_frustum.pos;

	LookAt(float3::zero); 

	GenerateMatrices();
}

void ModuleCamera::OrbitY(const float angle)
{
	if (!is_orbiting)
	{
		return;
	}

	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	const float current_angle = asinf(camera_frustum.front.y / camera_frustum.front.Length());
	if (abs(current_angle + adjusted_angle) >= math::pi / 2) {
		return;
	}

	float3x3 rotation_matrix = float3x3::identity;
	rotation_matrix.SetRotatePart(camera_frustum.WorldRight(), adjusted_angle);
	camera_frustum.pos = rotation_matrix * camera_frustum.pos;

	LookAt(float3::zero);

	GenerateMatrices();
}

void ModuleCamera::SetOrbit(const bool is_orbiting)
{
	this->is_orbiting = is_orbiting;
}

void ModuleCamera::SetSpeedUp(const bool is_speeding_up)
{
	speed_up = is_speeding_up ? SPEED_UP_FACTOR : 1.f;
}

void ModuleCamera::SetMovement(const bool movement_enabled)
{
	this->movement_enabled = movement_enabled;
}

bool ModuleCamera::MovementEnabled() const
{
	return movement_enabled;
}

void ModuleCamera::GenerateMatrices()
{
	proj = camera_frustum.ProjectionMatrix();
	view = camera_frustum.ViewMatrix();
}

void ModuleCamera::ShowCameraOptions()
{
	if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera"))
	{
		ImGui::DragFloat3("Front", &camera_frustum.front[0], NULL, NULL, NULL);
		ImGui::DragFloat3("Up", &camera_frustum.up[0], NULL, NULL, NULL);
		ImGui::DragFloat3("Position", &camera_frustum.pos[0], NULL, NULL, NULL);

		ImGui::Separator();

		ImGui::DragFloat("Mov Speed", &camera_movement_speed, NULL, NULL, NULL);
		ImGui::DragFloat("Rot Speed", &camera_rotation_speed, NULL, NULL, NULL);
		
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