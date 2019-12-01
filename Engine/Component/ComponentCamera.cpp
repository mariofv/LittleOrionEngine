#include "ComponentCamera.h"
#include "Application.h"
#include "Module/ModuleTime.h"
#include "GameObject.h"

#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>

ComponentCamera::ComponentCamera(GameObject * owner) : Component(owner, ComponentType::CAMERA)
{
	aspect_ratio = 1.f;
	camera_frustum.type = FrustumType::PerspectiveFrustum;
	camera_frustum.pos = float3::unitX;
	camera_frustum.front = -float3::unitZ;
	camera_frustum.up = float3::unitY;
	camera_frustum.nearPlaneDistance = 1.f;
	camera_frustum.farPlaneDistance = 100.0f;
	camera_frustum.verticalFov = math::pi / 4.0f;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);

	GenerateMatrices();
}

void ComponentCamera::Enable()
{

}

void ComponentCamera::Disable()
{

}

void ComponentCamera::Update()
{

}


void ComponentCamera::SetFOV(const float fov)
{
	camera_frustum.verticalFov = fov;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);

	GenerateMatrices();
}

void ComponentCamera::SetAspectRatio(const float aspect_ratio)
{
	this->aspect_ratio = aspect_ratio;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);

	GenerateMatrices();
}

void ComponentCamera::SetNearDistance(const float distance)
{
	camera_frustum.nearPlaneDistance = distance;

	GenerateMatrices();
}

void ComponentCamera::SetFarDistance(const float distance)
{
	camera_frustum.farPlaneDistance = distance;

	GenerateMatrices();
}

void ComponentCamera::SetOrientation(const float3 orientation)
{
	float3x3 rotation_matrix = float3x3::LookAt(camera_frustum.front, orientation, camera_frustum.up, float3::unitY);
	camera_frustum.front = rotation_matrix * camera_frustum.front;
	camera_frustum.up = rotation_matrix * camera_frustum.up;

	GenerateMatrices();
}

void ComponentCamera::SetOrthographicSize(const float2 size)
{
	camera_frustum.orthographicWidth = size.x;
	camera_frustum.orthographicHeight = size.y;

	GenerateMatrices();
}

void ComponentCamera::LookAt(const float3 focus)
{
	float3 look_direction = (focus - camera_frustum.pos).Normalized();
	SetOrientation(look_direction);

	GenerateMatrices();
}

void ComponentCamera::LookAt(const float x, const float y, const float z)
{
	LookAt(float3(x, y, z));
}

void ComponentCamera::RotatePitch(const float angle)
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

void ComponentCamera::RotateYaw(const float angle)
{
	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	float3x3 rotation_matrix = float3x3::RotateY(adjusted_angle);
	camera_frustum.up = rotation_matrix * camera_frustum.up;
	camera_frustum.front = rotation_matrix * camera_frustum.front;

	GenerateMatrices();
}

void ComponentCamera::SetPerpesctiveView()
{
	camera_frustum.type = FrustumType::PerspectiveFrustum;
	GenerateMatrices();
}

void ComponentCamera::SetOrthographicView()
{
	camera_frustum.type = FrustumType::OrthographicFrustum;
	GenerateMatrices();
}

void ComponentCamera::GenerateMatrices()
{
	proj = camera_frustum.ProjectionMatrix();
	view = camera_frustum.ViewMatrix();
}


void ComponentCamera::ShowComponentWindow()
{
	if (ImGui::CollapsingHeader(ICON_FA_VIDEO " Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}
