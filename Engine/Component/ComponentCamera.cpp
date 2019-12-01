#include "ComponentCamera.h"
#include "Application.h"
#include "GameObject.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"

#include "imgui.h"
#include <FontAwesome5/IconsFontAwesome5.h>

ComponentCamera::ComponentCamera() : Component(nullptr, ComponentType::CAMERA)
{
	glGenFramebuffers(1, &fbo);

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

ComponentCamera::ComponentCamera(GameObject * owner) : Component(owner, ComponentType::CAMERA)
{
	glGenFramebuffers(1, &fbo);

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

ComponentCamera::~ComponentCamera()
{
	glDeleteTextures(1, &last_recorded_frame_texture);
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);
}

void ComponentCamera::Enable()
{

}

void ComponentCamera::Disable()
{

}

void ComponentCamera::Update()
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
}

void ComponentCamera::RecordFrame(const float width, const float height)
{
	if (last_width != width || last_height != height)
	{
		last_width = width;
		last_height = height;
		SetAspectRatio(width/height);
		GenerateFrameBuffers(width, height);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	App->renderer->Render(*this);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint ComponentCamera::GetLastRecordedFrame() const
{
	return last_recorded_frame_texture;
}


void ComponentCamera::GenerateFrameBuffers(const float width, const float height)
{
	if (last_recorded_frame_texture != 0)
	{
		glDeleteTextures(1, &last_recorded_frame_texture);
	}

	glGenTextures(1, &last_recorded_frame_texture);

	if (rbo != 0)
	{
		glDeleteRenderbuffers(1, &rbo);
	}
	glGenRenderbuffers(1, &rbo);


	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, last_recorded_frame_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, last_recorded_frame_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void ComponentCamera::AlignOrientationWithAxis()
{
	camera_frustum.up = float3::unitY;
	camera_frustum.front = float3::unitZ;

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

void ComponentCamera::SetPosition(const float3 position)
{
	camera_frustum.pos = position;

	GenerateMatrices();
}

void ComponentCamera::Center(const AABB &bounding_box)
{
	camera_frustum.up = float3::unitY;
	camera_frustum.front = float3::unitZ;

	float containing_sphere_radius = bounding_box.Size().Length() / 2;

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

void ComponentCamera::Focus(const AABB &bounding_box)
{
	LookAt(bounding_box.CenterPoint());

	float containing_sphere_radius = bounding_box.Size().Length() / 2;
	is_focusing = true;
	desired_focus_position = bounding_box.CenterPoint() - BOUNDING_BOX_DISTANCE_FACTOR * containing_sphere_radius * camera_frustum.front;

	GenerateMatrices();
}


void ComponentCamera::MoveUp()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	float3 new_camera_pos = camera_frustum.pos;
	new_camera_pos.y = new_camera_pos.y + distance;
	camera_frustum.pos = new_camera_pos;

	GenerateMatrices();
}

void ComponentCamera::MoveDown()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	float3 new_camera_pos = camera_frustum.pos;
	new_camera_pos.y = new_camera_pos.y - distance;
	camera_frustum.pos = new_camera_pos;

	GenerateMatrices();
}

void ComponentCamera::MoveFoward()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	camera_frustum.pos += camera_frustum.front.ScaledToLength(distance);

	GenerateMatrices();
}

void ComponentCamera::MoveBackward()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	camera_frustum.pos -= camera_frustum.front.ScaledToLength(distance);

	GenerateMatrices();
}

void ComponentCamera::MoveLeft()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	camera_frustum.pos -= camera_frustum.WorldRight().ScaledToLength(distance);

	GenerateMatrices();
}

void ComponentCamera::MoveRight()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	camera_frustum.pos = camera_frustum.pos + camera_frustum.WorldRight().ScaledToLength(distance);

	GenerateMatrices();
}

void ComponentCamera::OrbitX(const float angle)
{
	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	float3x3 rotation_matrix = float3x3::RotateY(adjusted_angle);
	camera_frustum.pos = rotation_matrix * camera_frustum.pos;

	LookAt(float3::zero);

	GenerateMatrices();
}

void ComponentCamera::OrbitY(const float angle)
{
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

void ComponentCamera::SetSpeedUp(const bool is_speeding_up)
{
	speed_up = is_speeding_up ? SPEED_UP_FACTOR : 1.f;
}

float4x4 ComponentCamera::GetViewMatrix() const
{
	return view;
}

float4x4 ComponentCamera::GetProjectionMatrix() const
{
	return proj;
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
		ImGui::DragFloat3("Front", &camera_frustum.front[0], NULL, NULL, NULL);
		ImGui::DragFloat3("Up", &camera_frustum.up[0], NULL, NULL, NULL);
		ImGui::DragFloat3("Position", &camera_frustum.pos[0], NULL, NULL, NULL);

		ImGui::Separator();

		ImGui::SliderFloat("Mov Speed", &camera_movement_speed, CAMERA_MINIMUN_MOVEMENT_SPEED, CAMERA_MAXIMUN_MOVEMENT_SPEED);
		ImGui::SliderFloat("Rot Speed", &camera_rotation_speed, CAMERA_MINIMUN_MOVEMENT_SPEED, CAMERA_MAXIMUN_MOVEMENT_SPEED);
		ImGui::SliderFloat("Zoom Speed", &camera_zooming_speed, CAMERA_MINIMUN_MOVEMENT_SPEED, CAMERA_MAXIMUN_MOVEMENT_SPEED);

		if (ImGui::SliderFloat("FOV", &camera_frustum.horizontalFov, 0, 2 * 3.14f))
		{
			SetFOV(camera_frustum.horizontalFov);
		}

		if (ImGui::SliderFloat("Aspect Ratio", &aspect_ratio, 0, 10))
		{
			SetAspectRatio(aspect_ratio);
		}

		if (ImGui::SliderFloat("Near plane", &camera_frustum.nearPlaneDistance, 1, camera_frustum.farPlaneDistance + 1))
		{
			SetNearDistance(camera_frustum.nearPlaneDistance);
		}

		if (ImGui::SliderFloat("Far plane", &camera_frustum.farPlaneDistance, camera_frustum.nearPlaneDistance + 1, camera_frustum.nearPlaneDistance + 1000))
		{
			SetFarDistance(camera_frustum.farPlaneDistance);
		}

		if (ImGui::SliderFloat("Orthographic Size", &camera_frustum.orthographicHeight, 0, 100))
		{
			SetOrthographicSize(float2(camera_frustum.orthographicHeight * aspect_ratio, camera_frustum.orthographicHeight));
		}

		if (ImGui::Combo("Front faces", &perpesctive_enable, "Perspective\0Orthographic\0"))
		{
			switch (perpesctive_enable)
			{
			case 0:
				SetPerpesctiveView();
				break;
			case 1:
				SetOrthographicView();
				break;
			}
		}
	}
}
