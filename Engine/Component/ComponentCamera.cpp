#include "ComponentCamera.h"

#include "Helper/Utils.h"

#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleAI.h"
#include "Module/ModuleCamera.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleDebugDraw.h"
#include "Module/ModuleEditor.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleRender.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleTime.h"
#include "Module/ModuleUI.h"
#include "Module/ModuleWindow.h"

#include "ResourceManagement/Resources/Skybox.h"
#include "ResourceManagement/ResourcesDB/CoreResources.h"

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

ComponentCamera & ComponentCamera::operator=(const ComponentCamera & component_to_copy)
{
	Component::operator = (component_to_copy);
	this->camera_frustum = component_to_copy.camera_frustum;

	this->camera_clear_mode = component_to_copy.camera_clear_mode;
	this->camera_clear_color, component_to_copy.camera_clear_color;
	this->depth = component_to_copy.depth;
	this->camera_movement_speed = component_to_copy.camera_movement_speed;
	this->speed_up = component_to_copy.speed_up;

	GenerateMatrices();
	return *this;
}
Component* ComponentCamera::Clone(GameObject* owner, bool original_prefab)
{ 
	ComponentCamera * created_component;
	if (original_prefab)
	{
		created_component = new ComponentCamera();
	}
	else
	{
		created_component = App->cameras->CreateComponentCamera();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
	return created_component;
}

void ComponentCamera::CopyTo(Component* component_to_copy) const
{  
	*component_to_copy = *this;
	*static_cast<ComponentCamera*>(component_to_copy) = *this;
}

bool ComponentCamera::HasSkybox() const
{
	return camera_clear_mode == ClearMode::SKYBOX;
}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::InitCamera()
{
	aspect_ratio = 1.F;
	camera_frustum.type = FrustumType::PerspectiveFrustum;
	camera_frustum.pos = float3::unitX;
	camera_frustum.front = float3::unitZ;
	camera_frustum.up = float3::unitY;
	camera_frustum.nearPlaneDistance = 1.F;
	camera_frustum.farPlaneDistance = 100.0F;
	camera_frustum.verticalFov = math::pi / 4.0F;
	camera_frustum.horizontalFov = 2.F * atanf(tanf(camera_frustum.verticalFov * 0.5F) * aspect_ratio);
}

void ComponentCamera::Update()
{
#if !GAME
	if (is_focusing)
	{
		float focus_progress = math::Min((App->time->real_time_since_startup - start_focus_time) / CENTER_TIME, 1.f);
		assert(focus_progress >= 0 && focus_progress <= 1.f);
		float3 new_camera_position = start_focus_position.Lerp(goal_focus_position, focus_progress);
		owner->transform.SetTranslation(new_camera_position);
		is_focusing = focus_progress != 1;
	}	
#endif

	camera_frustum.pos = owner->transform.GetGlobalTranslation();
	Quat owner_rotation = owner->transform.GetGlobalRotation();
	camera_frustum.up = owner_rotation * float3::unitY;
	camera_frustum.front = owner_rotation * float3::unitZ;

	GenerateMatrices();
}

void ComponentCamera::Delete()
{
	App->cameras->RemoveComponentCamera(this);
}

void ComponentCamera::SpecializedSave(Config& config) const
{
	config.AddUInt(camera_frustum.type, "FrustumType");
	config.AddFloat(camera_frustum.nearPlaneDistance, "NearPlaneDistance");
	config.AddFloat(camera_frustum.farPlaneDistance, "FarPlaneDistance");
	config.AddFloat(camera_frustum.verticalFov, "VerticalFOV");
	config.AddUInt((uint64_t)camera_clear_mode, "ClearMode");
	config.AddFloat3(camera_clear_color, "Clear Color");
	config.AddInt(depth, "Depth");

	config.AddUInt(skybox_uuid, "Skybox");
}

void ComponentCamera::SpecializedLoad(const Config& config)
{
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

	config.GetFloat3("Clear Color", camera_clear_color, float3::zero);
	
	depth = config.GetInt("Depth", 0);

	skybox_uuid = config.GetUInt32("Skybox", 0);
	SetSkybox(skybox_uuid);

	GenerateMatrices();
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

float ComponentCamera::GetNearDistance() const
{
	return camera_frustum.nearPlaneDistance;
}

void ComponentCamera::SetFarDistance(float distance)
{
	camera_frustum.farPlaneDistance = distance;
}

float ComponentCamera::GetFarDistance() const
{
	return camera_frustum.farPlaneDistance;
}

void ComponentCamera::SetOrientation(const float3 & orientation)
{
	Quat rotation = Quat::LookAt(owner->transform.GetFrontVector(), orientation, owner->transform.GetUpVector(), float3::unitY);

	owner->transform.Rotate(rotation);
}

void ComponentCamera::SetStartFocusPosition(const float3& focus_position)
{
	start_focus_position = focus_position;
}

void ComponentCamera::SetGoalFocusPosition(const float3& focus_position)
{
	goal_focus_position = focus_position;
}

void ComponentCamera::SetFocusTime(const float focus_time)
{
	start_focus_time = focus_time;
}

ENGINE_API Frustum ComponentCamera::GetFrustum()
{
	return camera_frustum;
}

void ComponentCamera::AlignOrientationWithAxis()
{
	float3x3 rotation_matrix = float3x3::identity;
	owner->transform.SetRotation(rotation_matrix);
}

ENGINE_API void ComponentCamera::SetOrthographicSize(const float2 & size)
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

void ComponentCamera::CenterGame(const GameObject* go)
{
	float containing_sphere_radius = go->aabb.bounding_box.Size().Length() / 2 ;
	is_focusing = true;
	start_focus_position = owner->transform.GetTranslation();
	goal_focus_position = go->aabb.bounding_box.CenterPoint() - camera_frustum.front * BOUNDING_BOX_DISTANCE_FACTOR * containing_sphere_radius;;
	start_focus_time = App->time->delta_time;
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

void ComponentCamera::MoveForward()
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
	if (abs(current_angle + adjusted_angle) >= math::pi / 2) 
	{
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
	if (abs(current_angle + adjusted_angle) >= math::pi / 2) 
	{ // Avoid Gimbal Lock
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

void ComponentCamera::SetSkybox(uint32_t skybox_uuid)
{
	this->skybox_uuid = skybox_uuid;
	if (skybox_uuid != 0)
	{
		camera_skybox = App->resources->Load<Skybox>(skybox_uuid);
	}
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

float4x4 ComponentCamera::GetClipMatrix() const
{
	return proj * view;
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

bool ComponentCamera::IsInsideFrustum(const Frustum& frustum, const AABB& aabb)
{
	return CheckAABBCollision(frustum, aabb) != ComponentAABB::CollisionState::OUTSIDE;
}

bool ComponentCamera::IsInsideFrustum(const AABB2D& aabb) const
{
	return CheckAABB2DCollision(aabb) != ComponentAABB::CollisionState::OUTSIDE;
}

bool ComponentCamera::IsInsideFrustum(const Frustum& frustum, const AABB2D& aabb)
{
	return CheckAABB2DCollision(frustum, aabb) != ComponentAABB::CollisionState::OUTSIDE;
}

bool ComponentCamera::IsCompletlyInsideFrustum(const AABB& aabb) const
{
	return CheckAABBCollision(aabb) == ComponentAABB::CollisionState::INSIDE;
}

ComponentAABB::CollisionState ComponentCamera::CheckAABBCollision(const AABB& reference_AABB) const
{
	return CheckAABBCollision(camera_frustum, reference_AABB);
}

ComponentAABB::CollisionState ComponentCamera::CheckAABBCollision(const Frustum& frustum, const AABB& reference_AABB)
{
	static const size_t number_of_corners = 8;
	static const size_t number_of_planes = 6;

	//Get refence corners
	float3 reference_aabb_corners[number_of_corners];
	reference_AABB.GetCornerPoints(reference_aabb_corners);

	//Get own aabb planes
	Plane own_frustum_planes[number_of_planes];
	frustum.GetPlanes(own_frustum_planes);

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
	return CheckAABB2DCollision(camera_frustum, reference_AABB);
}

ComponentAABB::CollisionState ComponentCamera::CheckAABB2DCollision(const Frustum& frustum, const AABB2D& reference_AABB)
{
	static const size_t number_of_corners = 4;
	static const size_t number_of_planes = 4;

	//Get own aabb planes
	Plane own_frustum_planes[6];
	frustum.GetPlanes(own_frustum_planes);

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

void ComponentCamera::GetRay(const float2 &mouse_position, LineSegment &return_value) const
{
	float2 normalized_position = float2::zero;
#if GAME
	float2 window_mouse_position = mouse_position - float2(App->window->GetWidth()/2, App->window->GetHeight()/2);
	normalized_position = float2(window_mouse_position.x * 2 / App->window->GetWidth(), -window_mouse_position.y * 2 / App->window->GetHeight());
#else
	if (App->time->isGameRunning() && App->editor->game_panel->IsHovered())
	{
		float2 window_center_pos = App->editor->game_panel->game_window_content_area_pos + float2(App->editor->game_panel->game_window_content_area_width, App->editor->game_panel->game_window_content_area_height) / 2;

		float2 window_mouse_position = mouse_position - window_center_pos;
		normalized_position = float2(window_mouse_position.x * 2 / App->editor->game_panel->game_window_content_area_width, -window_mouse_position.y * 2 / App->editor->game_panel->game_window_content_area_height);
	}
	else
	{
		float2 window_center_pos = App->editor->scene_panel->scene_window_content_area_pos + float2(App->editor->scene_panel->scene_window_content_area_width, App->editor->scene_panel->scene_window_content_area_height) / 2;

		float2 window_mouse_position = mouse_position - window_center_pos;
		normalized_position = float2(window_mouse_position.x * 2 / App->editor->scene_panel->scene_window_content_area_width, -window_mouse_position.y * 2 / App->editor->scene_panel->scene_window_content_area_height);
	}
#endif
	return_value = camera_frustum.UnProjectLineSegment(normalized_position.x, normalized_position.y);
}

AABB ComponentCamera::GetMinimalEnclosingAABB() const
{
	return camera_frustum.MinimalEnclosingAABB();
}

