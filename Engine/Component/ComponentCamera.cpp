#include "ComponentCamera.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleWindow.h"

#include "Helper/Utils.h"

ComponentCamera::ComponentCamera() : Component(nullptr, ComponentType::CAMERA)
{
	InitCamera();
	GenerateMatrices();
}

ComponentCamera::ComponentCamera(GameObject * owner) : Component(owner, ComponentType::CAMERA)
{
	InitCamera();
	GenerateMatrices();
}

ComponentCamera::~ComponentCamera()
{
	glDeleteTextures(1, &last_recorded_frame_texture);
	glDeleteTextures(1, &msfb_color);

	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &fbo);
	glDeleteFramebuffers(1, &msfbo);
}

void ComponentCamera::InitCamera()
{
	glGenFramebuffers(1, &fbo);
	glGenFramebuffers(1, &msfbo);

	aspect_ratio = 1.f;
	camera_frustum.type = FrustumType::PerspectiveFrustum;
	camera_frustum.pos = float3::unitX;
	camera_frustum.front = float3::unitZ;
	camera_frustum.up = float3::unitY;
	camera_frustum.nearPlaneDistance = 1.f;
	camera_frustum.farPlaneDistance = 100.0f;
	camera_frustum.verticalFov = math::pi / 4.0f;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);
}

void ComponentCamera::Update()
{
	if (is_focusing)
	{
		float focus_progress = math::Min((App->time->real_time_since_startup - start_focus_time) / CENTER_TIME, 1.f);
		assert(focus_progress >= 0 && focus_progress <= 1.f);
		float3 new_camera_position = start_focus_position.Lerp(goal_focus_position, focus_progress);
		owner->transform.SetTranslation(new_camera_position);
		is_focusing = focus_progress != 1;
	}	

	camera_frustum.pos = owner->transform.GetTranslation();
	Quat owner_rotation = owner->transform.GetRotation();
	camera_frustum.up = owner_rotation * float3::unitY;
	camera_frustum.front = owner_rotation * float3::unitZ;


	GenerateMatrices();
}

void ComponentCamera::Delete()
{
	App->cameras->RemoveComponentCamera(this);
}

void ComponentCamera::Save(Config& config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddUInt((uint64_t)type, "ComponentType");
	config.AddBool(active, "Active");
	config.AddUInt(camera_frustum.type, "FrustumType");
	config.AddFloat(camera_frustum.nearPlaneDistance, "NearPlaneDistance");
	config.AddFloat(camera_frustum.farPlaneDistance, "FarPlaneDistance");
	config.AddFloat(camera_frustum.verticalFov, "VerticalFOV");
	config.AddUInt((uint64_t)camera_clear_mode, "ClearMode");
	config.AddColor(float4(camera_clear_color[0], camera_clear_color[1], camera_clear_color[2], 1.f), "ClearColor");
	config.AddInt(depth, "Depth");
}

void ComponentCamera::Load(const Config& config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	uint64_t frustum_type_int = config.GetUInt("FrustumType", 1);
	switch (frustum_type_int)
	{
	case 0:
		camera_frustum.type = math::InvalidFrustum;
		break;
	case 1:
		camera_frustum.type = math::OrthographicFrustum;
		break;
	case 2:
		camera_frustum.type = math::PerspectiveFrustum;
		break;
	}
	camera_frustum.nearPlaneDistance = config.GetFloat("NearPlaneDistance", 1.0f);
	camera_frustum.farPlaneDistance = config.GetFloat("FarPlaneDistance", 100.0f);
	camera_frustum.verticalFov = config.GetFloat("VerticalFOV", math::pi / 4.0f);
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);

	uint64_t camera_clear_mode_uint = config.GetUInt("ClearMode", 0);
	switch (camera_clear_mode_uint)
	{
	case 0:
		camera_clear_mode = ClearMode::COLOR;
		break;
	case 1:
		camera_clear_mode = ClearMode::SKYBOX;
		break;
	}

	float4 clear_color;
	config.GetColor("ClearColor", clear_color, float4(0.f, 0.f, 0.f, 1.f));
	camera_clear_color[0] = clear_color.x;
	camera_clear_color[1] = clear_color.y;
	camera_clear_color[2] = clear_color.z;

	depth = config.GetInt("Depth", 0);

	GenerateMatrices();
}

float ComponentCamera::GetWidth() const
{
	return last_width;
}

float ComponentCamera::GetHeigt() const
{
	return last_height;
}

void ComponentCamera::RecordFrame(float width, float height)
{
	if (last_width != width || last_height != height || toggle_msaa)
	{
		last_width = width;
		last_height = height;
		SetAspectRatio(width/height);
		GenerateFrameBuffers(width, height);
		toggle_msaa = false;
	}

	App->renderer->anti_aliasing ? glBindFramebuffer(GL_FRAMEBUFFER, msfbo) : glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, width, height);

	switch (camera_clear_mode)
	{
		case ComponentCamera::ClearMode::COLOR:
			glClearColor(camera_clear_color[0], camera_clear_color[1], camera_clear_color[2], 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(0.f, 0.f, 0.f, 1.f);
			break;
		case ComponentCamera::ClearMode::SKYBOX:
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			App->cameras->skybox->Render(*this);
			break;
		default:
			break;
	}

	App->renderer->RenderFrame(*this);

	if (App->renderer->anti_aliasing)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msfbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentCamera::RecordDebugDraws(float width, float height) const
{
	App->renderer->anti_aliasing ? glBindFramebuffer(GL_FRAMEBUFFER, msfbo) : glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, width, height);

	App->debug_draw->Render();

	if (App->renderer->anti_aliasing)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msfbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint ComponentCamera::GetLastRecordedFrame() const
{
	return last_recorded_frame_texture;
}

void ComponentCamera::GenerateFrameBuffers(float width, float height)
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

	App->renderer->anti_aliasing ? CreateMssaFramebuffer(width, height) : CreateFramebuffer(width, height);
}

void ComponentCamera::CreateFramebuffer(float width, float height)
{
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, last_recorded_frame_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, last_recorded_frame_texture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentCamera::CreateMssaFramebuffer(float width, float height)
{
	glGenTextures(1, &msfb_color);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msfb_color);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, msfbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msfb_color, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, last_recorded_frame_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, last_recorded_frame_texture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentCamera::SetFOV(float fov)
{
	camera_frustum.verticalFov = fov;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);
}

void ComponentCamera::SetAspectRatio(float aspect_ratio)
{
	this->aspect_ratio = aspect_ratio;
	camera_frustum.horizontalFov = 2.f * atanf(tanf(camera_frustum.verticalFov * 0.5f) * aspect_ratio);
}

void ComponentCamera::SetNearDistance(float distance)
{
	camera_frustum.nearPlaneDistance = distance;
}

void ComponentCamera::SetFarDistance(float distance)
{
	camera_frustum.farPlaneDistance = distance;
}

void ComponentCamera::SetOrientation(const float3 & orientation)
{
	Quat rotation = Quat::LookAt(owner->transform.GetFrontVector(), orientation, owner->transform.GetUpVector(), float3::unitY);

	owner->transform.Rotate(rotation);
}

void ComponentCamera::AlignOrientationWithAxis()
{
	float3x3 rotation_matrix = float3x3::identity;
	owner->transform.SetRotation(rotation_matrix);
}

void ComponentCamera::SetOrthographicSize(const float2 & size)
{
	camera_frustum.orthographicWidth = size.x;
	camera_frustum.orthographicHeight = size.y;
}

void ComponentCamera::LookAt(const float3 & focus)
{
	float3 look_direction = (focus - owner->transform.GetTranslation()).Normalized();
	SetOrientation(look_direction);
}

void ComponentCamera::LookAt(float x, float y, float z)
{
	LookAt(float3(x, y, z));
}

void ComponentCamera::SetPosition(const float3 & position)
{
	owner->transform.SetTranslation(position);
}

void ComponentCamera::Center(const AABB &bounding_box)
{
	float containing_sphere_radius = bounding_box.Size().Length() / 2;

	// Move camera position to visualize the whole bounding box
	is_focusing = true;
	start_focus_position = owner->transform.GetTranslation();
	goal_focus_position = bounding_box.CenterPoint() - camera_frustum.front * BOUNDING_BOX_DISTANCE_FACTOR * containing_sphere_radius;;
	start_focus_time = App->time->real_time_since_startup;
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

void ComponentCamera::OrbitCameraWithMouseMotion(const float2 &motion, const float3& focus_point)
{
	if (math::Abs(motion.y) > 1.5)
	{
		App->cameras->scene_camera->OrbitY(motion.y, focus_point);
	}

	if (math::Abs(motion.x) > 1.5)
	{
		App->cameras->scene_camera->OrbitX(motion.x, focus_point);
	}
}

void ComponentCamera::OrbitX(float angle, const float3& focus_point)
{
	float3 cam_focus_vector = owner->transform.GetTranslation() - focus_point;

	const float adjusted_angle = App->time->real_time_delta_time * CAMERA_ROTATION_SPEED * -angle;
	Quat rotation = Quat::RotateY(adjusted_angle);

	cam_focus_vector = rotation * cam_focus_vector;
	owner->transform.SetTranslation(cam_focus_vector + focus_point);

	LookAt(focus_point);
}

void ComponentCamera::OrbitY(float angle, const float3& focus_point)
{
	float3 cam_focus_vector = owner->transform.GetTranslation() - focus_point;

	const float adjusted_angle = App->time->real_time_delta_time * CAMERA_ROTATION_SPEED * -angle;
	const float current_angle = asinf(owner->transform.GetFrontVector().y / owner->transform.GetFrontVector().Length());
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

	if (math::Abs(motion.x) > 1.5)
	{
		App->cameras->scene_camera->RotateYaw(motion.x);
	}

	if (math::Abs(motion.y) > 1.5) 
	{
		App->cameras->scene_camera->RotatePitch(motion.y);
	}
}

void ComponentCamera::RotatePitch(float angle)
{
	const float adjusted_angle = App->time->real_time_delta_time * CAMERA_ROTATION_SPEED * -angle;
	const float current_angle = asinf(owner->transform.GetFrontVector().y / owner->transform.GetFrontVector().Length());
	if (abs(current_angle + adjusted_angle) >= math::pi / 2) { // Avoid Gimbal Lock
		return;
	}
	Quat rotation = Quat::RotateAxisAngle(owner->transform.GetRightVector(), adjusted_angle);
	owner->transform.Rotate(rotation);
}

void ComponentCamera::RotateYaw(float angle)
{
	const float adjusted_angle = App->time->real_time_delta_time * CAMERA_ROTATION_SPEED * -angle;
	Quat rotation = Quat::RotateY(adjusted_angle);
	owner->transform.Rotate(rotation);
}

void ComponentCamera::SetPerpesctiveView()
{
	camera_frustum.type = FrustumType::PerspectiveFrustum;
}

void ComponentCamera::SetOrthographicView()
{
	camera_frustum.type = FrustumType::OrthographicFrustum;
}

void ComponentCamera::SetClearMode(ComponentCamera::ClearMode clear_mode)
{
	camera_clear_mode = clear_mode;
}

void ComponentCamera::SetSpeedUp(bool is_speeding_up)
{
	speed_up = is_speeding_up ? SPEED_UP_FACTOR : 1.f;
}

void ComponentCamera::SetViewMatrix(const float4x4& view_matrix)
{
	float3x4 reduced_view_matrix;
	reduced_view_matrix.SetRow(0, view_matrix.Row3(0).ptr());
	reduced_view_matrix.SetRow(1, view_matrix.Row3(1).ptr());
	reduced_view_matrix.SetRow(2, view_matrix.Row3(2).ptr());

	reduced_view_matrix.InverseOrthonormal(); // Transformation to world matrix
	float3 front = -reduced_view_matrix.Col3(2);
	
	Quat rotation = Quat::LookAt(owner->transform.GetFrontVector(), front, owner->transform.GetUpVector(), float3::unitY);
	owner->transform.Rotate(rotation);
	
}

float4x4 ComponentCamera::GetViewMatrix() const
{
	return view;
}

float4x4 ComponentCamera::GetProjectionMatrix() const
{
	return proj;
}

float4x4 ComponentCamera::GetInverseClipMatrix() const
{
	return camera_frustum.ViewProjMatrix().Inverted();
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

bool ComponentCamera::IsInsideFrustum(const AABB2D& aabb) const
{
	return CheckAABB2DCollision(aabb) != ComponentAABB::CollisionState::OUTSIDE;
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
	if (total_reference_planes_inside == number_of_planes)
	{
		return ComponentAABB::CollisionState::INSIDE;
	}
	// we must be partly in then otherwise
	return ComponentAABB::CollisionState::INTERSECT;
}

ComponentAABB::CollisionState ComponentCamera::CheckAABB2DCollision(const AABB2D& reference_AABB) const
{
	static const size_t number_of_corners = 4;
	static const size_t number_of_planes = 4;

	//Get own aabb planes
	Plane own_frustum_planes[6];
	camera_frustum.GetPlanes(own_frustum_planes);
	
	//Get refence corners
	std::vector<float> reference_aabb_corners = Utils::GetVertices(reference_AABB);

	//Check if Corners are inside the planes
	int total_reference_planes_inside = 0;
	for (int p = 0; p < number_of_planes; ++p)
	{
		int points_inside_count = number_of_corners;
		int is_plane_inside = 1;
		for (int i = 0; i < number_of_corners; ++i)
		{
			float3 current_corner = float3(reference_aabb_corners[3 * i], reference_aabb_corners[3 * i + 1], reference_aabb_corners[3 * i + 2]);
			if (own_frustum_planes[p].IsOnPositiveSide(current_corner)) //If true, the point is halfway or outside
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
	if (total_reference_planes_inside == number_of_planes)
	{
		return ComponentAABB::CollisionState::INSIDE;
	}
	// we must be partly in then otherwise
	return ComponentAABB::CollisionState::INTERSECT;
}

void ComponentCamera::GetRay(const float2& normalized_position, LineSegment &return_value) const
{
	return_value = camera_frustum.UnProjectLineSegment(normalized_position.x, normalized_position.y);
}

AABB ComponentCamera::GetMinimalEnclosingAABB() const
{
	return camera_frustum.MinimalEnclosingAABB();
}
