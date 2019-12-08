#include "ComponentCamera.h"
#include "Application.h"
#include "GameObject.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleCamera.h"

ComponentCamera::ComponentCamera() : Component(nullptr, ComponentType::CAMERA)
{
	glGenFramebuffers(1, &fbo);

	aspect_ratio = 1.f;
	camera_frustum.type = FrustumType::PerspectiveFrustum;
	camera_frustum.pos = float3::unitX;
	camera_frustum.front = float3::unitZ;
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
	camera_frustum.front = float3::unitZ;
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

	//App->cameras->RemoveComponentCamera(this);
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
		float3 zooming_direction = desired_focus_position - owner->transform.GetTranslation();
		float distance_to_desired_zooming_position = zooming_direction.Length();
		float frame_focusing_distance = App->time->real_time_delta_time * camera_zooming_speed;
		if (distance_to_desired_zooming_position - frame_focusing_distance < 0)
		{
			owner->transform.SetTranslation(desired_focus_position);
			is_focusing = false;
		}
		else
		{
			owner->transform.SetTranslation(owner->transform.GetTranslation() + zooming_direction.ScaledToLength(frame_focusing_distance));
		}
	}	

	camera_frustum.pos = owner->transform.GetTranslation();
	Quat owner_rotation = owner->transform.GetRotation();
	camera_frustum.up = owner_rotation * float3::unitY;
	camera_frustum.front = owner_rotation * float3::unitZ;


	GenerateMatrices();
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

	switch (camera_clear_mode)
	{
		case ComponentCamera::ClearMode::COLOR:
			glClearColor(camera_clear_color[0], camera_clear_color[1], camera_clear_color[2], 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.f, 0.f, 0.f, 1.f);
			break;
		case ComponentCamera::ClearMode::SKYBOX:
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			App->cameras->skybox->Render(*this);
			break;
		default:
			break;
	}


	App->renderer->RenderFrame(*this);

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
}

void ComponentCamera::SetAspectRatio(const float aspect_ratio)
{
	this->aspect_ratio = aspect_ratio;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);
}

void ComponentCamera::SetNearDistance(const float distance)
{
	camera_frustum.nearPlaneDistance = distance;
}

void ComponentCamera::SetFarDistance(const float distance)
{
	camera_frustum.farPlaneDistance = distance;
}

void ComponentCamera::SetOrientation(const float3 orientation)
{
	Quat rotation = Quat::LookAt(camera_frustum.front, orientation, camera_frustum.up, float3::unitY);

	owner->transform.Rotate(rotation);
}

void ComponentCamera::AlignOrientationWithAxis()
{
	float3x3 rotation_matrix = float3x3::identity;
	owner->transform.SetRotation(rotation_matrix);
}


void ComponentCamera::SetOrthographicSize(const float2 size)
{
	camera_frustum.orthographicWidth = size.x;
	camera_frustum.orthographicHeight = size.y;
}

void ComponentCamera::LookAt(const float3 focus)
{
	float3 look_direction = (focus - owner->transform.GetTranslation()).Normalized();
	SetOrientation(look_direction);
}

void ComponentCamera::LookAt(const float x, const float y, const float z)
{
	LookAt(float3(x, y, z));
}

void ComponentCamera::SetPosition(const float3 position)
{
	owner->transform.SetTranslation(position);
}

void ComponentCamera::Center(const AABB &bounding_box)
{
	AlignOrientationWithAxis();

	float containing_sphere_radius = bounding_box.Size().Length() / 2;

	// Adapt camera speed to bounding box size
	camera_movement_speed = CAMERA_MOVEMENT_SPEED_BOUNDING_BOX_RADIUS_FACTOR * containing_sphere_radius;
	camera_zooming_speed = CAMERA_ZOOMING_SPEED_BOUNDING_BOX_RADIUS_FACTOR * containing_sphere_radius;

	// Adapt far plane to visualize the whole bounding box
	camera_frustum.farPlaneDistance = FAR_PLANE_FACTOR * containing_sphere_radius;

	// Move camera position to visualize the whole bounding box
	float3 new_camera_pos = bounding_box.CenterPoint() - camera_frustum.front * BOUNDING_BOX_DISTANCE_FACTOR * containing_sphere_radius;
	new_camera_pos.y = INITIAL_HEIGHT_FACTOR * containing_sphere_radius;
	owner->transform.SetTranslation(new_camera_pos);
}

void ComponentCamera::Focus(const AABB &bounding_box)
{
	LookAt(bounding_box.CenterPoint());

	float containing_sphere_radius = bounding_box.Size().Length() / 2;
	is_focusing = true;
	desired_focus_position = bounding_box.CenterPoint() - BOUNDING_BOX_DISTANCE_FACTOR * containing_sphere_radius * camera_frustum.front;
}


void ComponentCamera::MoveUp()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	owner->transform.Translate(float3(0, distance, 0));
}

void ComponentCamera::MoveDown()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	owner->transform.Translate(float3(0, -distance, 0));
}

void ComponentCamera::MoveFoward()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	owner->transform.Translate(camera_frustum.front.ScaledToLength(distance));
}

void ComponentCamera::MoveBackward()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	owner->transform.Translate(-camera_frustum.front.ScaledToLength(distance));
}

void ComponentCamera::MoveLeft()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	owner->transform.Translate(-camera_frustum.WorldRight().ScaledToLength(distance));
}

void ComponentCamera::MoveRight()
{
	const float distance = App->time->real_time_delta_time * camera_movement_speed * speed_up;
	owner->transform.Translate(camera_frustum.WorldRight().ScaledToLength(distance));
}

void ComponentCamera::OrbitCameraWithMouseMotion(const float2 &motion)
{
	if (math::Abs(motion.y) > 1.5)
	{
		App->cameras->scene_camera->OrbitY(motion.y);
	}

	if (math::Abs(motion.x) > 1.5)
	{
		App->cameras->scene_camera->OrbitX(motion.x);
	}
}

void ComponentCamera::OrbitX(const float angle)
{
	float3 focus_point = float3::zero;
	float3 cam_focus_vector = owner->transform.GetTranslation() - focus_point;

	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	Quat rotation = Quat::RotateY(adjusted_angle);

	cam_focus_vector = rotation * cam_focus_vector;
	owner->transform.SetTranslation(cam_focus_vector + focus_point);

	LookAt(focus_point);
}

void ComponentCamera::OrbitY(const float angle)
{
	float3 focus_point = float3::zero;
	float3 cam_focus_vector = owner->transform.GetTranslation() - focus_point;

	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	const float current_angle = asinf(camera_frustum.front.y / camera_frustum.front.Length());
	if (abs(current_angle + adjusted_angle) >= math::pi / 2) {
		return;
	}
	Quat rotation = Quat::RotateAxisAngle(camera_frustum.WorldRight(), adjusted_angle);

	cam_focus_vector = rotation * cam_focus_vector;
	owner->transform.SetTranslation(cam_focus_vector + focus_point);

	LookAt(focus_point);
}

void ComponentCamera::RotateCameraWithMouseMotion(const float2 &motion)
{
	Quat rotX, rotY;

	if (math::Abs(motion.y) > 1.5) 
	{
		rotX = App->cameras->scene_camera->RotatePitch(motion.y);
	}
	else
	{
		rotX = Quat::identity;
	}

	if (math::Abs(motion.x) > 1.5)
	{
		rotY = App->cameras->scene_camera->RotateYaw(motion.x);
	}
	else
	{
		rotY = Quat::identity;
	}

	owner->transform.Rotate(rotY * rotX);
}

Quat ComponentCamera::RotatePitch(const float angle)
{
	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	const float current_angle = asinf(camera_frustum.front.y / camera_frustum.front.Length());
	if (abs(current_angle + adjusted_angle) >= math::pi / 2) { // Avoid Gimbal Lock
		return Quat::identity;
	}
	Quat rotation = Quat::RotateAxisAngle(camera_frustum.WorldRight(), adjusted_angle);
	return rotation;
}

Quat ComponentCamera::RotateYaw(const float angle)
{
	const float adjusted_angle = App->time->real_time_delta_time * camera_rotation_speed * -angle;
	Quat rotation = Quat::RotateY(adjusted_angle);
	return rotation;
}

void ComponentCamera::SetPerpesctiveView()
{
	camera_frustum.type = FrustumType::PerspectiveFrustum;
}

void ComponentCamera::SetOrthographicView()
{
	camera_frustum.type = FrustumType::OrthographicFrustum;
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

std::vector<float> ComponentCamera::GetFrustumVertices() const
{
	float3 tmp_vertices[8];
	camera_frustum.GetCornerPoints(&tmp_vertices[0]);

	std::vector<float> vertices(24);
	for (unsigned int i = 0; i < 8; ++i)
	{
		vertices[i * 3] = tmp_vertices[i].x;
		vertices[i * 3 + 1] = tmp_vertices[i].y;
		vertices[i * 3 + 2] = tmp_vertices[i].z;
	}

	return vertices;
}

bool ComponentCamera::IsInsideFrustum(const AABB& aabb) const
{
	return CheckAABBCollision(aabb) != ComponentAABB::CollisionState::OUTSIDE;
}

ComponentAABB::CollisionState ComponentCamera::CheckAABBCollision(const AABB& reference_AABB) const
{
	static const size_t number_of_corners = 8;
	static const size_t number_of_planes = 6;

	//Get refence corners
	float3 reference_aabb_corners[number_of_corners];
	reference_AABB.GetCornerPoints(reference_aabb_corners);

	//Get own aabb planes
	Plane own_frustum_planes[number_of_planes];
	camera_frustum.GetPlanes(own_frustum_planes);

	//Check if Corners are inside the planes
	int total_reference_planes_inside = 0;
	for (int p = 0; p < number_of_planes; ++p)
	{
		int points_inside_count = number_of_corners;
		int is_plane_inside = 1;
		for (int i = 0; i < number_of_corners; ++i)
		{
			if (own_frustum_planes[p].IsOnPositiveSide(reference_aabb_corners[i])) //If true, the point is halfway or outside
			{
				// Plane is not inside
				is_plane_inside = 0;
				--points_inside_count;
			}
		}
		if (points_inside_count == 0)
		{
			return ComponentAABB::CollisionState::OUTSIDE;
		}
		total_reference_planes_inside += is_plane_inside;
	}
	// so if total_reference_planes_inside is 6, then all are inside the view
	if (total_reference_planes_inside == 6)
	{
		return ComponentAABB::CollisionState::INSIDE;
	}
	// we must be partly in then otherwise
	return ComponentAABB::CollisionState::INTERSECT;
}

void ComponentCamera::ShowComponentWindow()
{
	ComponentsUI::ShowComponentCameraWindow(this);
}
