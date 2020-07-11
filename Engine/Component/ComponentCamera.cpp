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
	memcpy(camera_clear_color, component_to_copy.camera_clear_color,3 * sizeof(float));
	this->depth = component_to_copy.depth;
	this->camera_movement_speed = component_to_copy.camera_movement_speed;
	this->toggle_msaa = component_to_copy.toggle_msaa;
	this->speed_up = component_to_copy.speed_up;

	GenerateMatrices();
	return *this;
}
Component* ComponentCamera::Clone(bool original_prefab) const
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
	return created_component;
};
void ComponentCamera::Copy(Component* component_to_copy) const
{  
	*component_to_copy = *this;
	*static_cast<ComponentCamera*>(component_to_copy) = *this;
};

ComponentCamera::~ComponentCamera()
{
	glDeleteTextures(1, &last_recorded_frame_texture);
	glDeleteTextures(1, &msfb_color);
	glDeleteTextures(1, &depth_map);


	glDeleteRenderbuffers(1, &rbo);
	glDeleteRenderbuffers(1, &depth_rbo);

	glDeleteFramebuffers(1, &fbo);
	glDeleteFramebuffers(1, &msfbo);
}

void ComponentCamera::InitCamera()
{
	glGenFramebuffers(1, &fbo);
	glGenFramebuffers(1, &msfbo);


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

	if (camera_frustum.type == FrustumType::OrthographicFrustum)
	{
		SetOrthographicSize(float2(camera_frustum.orthographicWidth, camera_frustum.orthographicHeight));
	}


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
	config.AddColor(float4(camera_clear_color[0], camera_clear_color[1], camera_clear_color[2], 1.f), "ClearColor");
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

	float4 clear_color;
	config.GetColor("ClearColor", clear_color, float4(0.f, 0.f, 0.f, 1.f));
	camera_clear_color[0] = clear_color.x;
	camera_clear_color[1] = clear_color.y;
	camera_clear_color[2] = clear_color.z;

	depth = config.GetInt("Depth", 0);

	skybox_uuid = config.GetUInt32("Skybox", 0);
	SetSkybox(skybox_uuid);

	GenerateMatrices();
}

float ComponentCamera::GetWidth() const
{
	return last_width;
}

float ComponentCamera::GetHeight() const
{
	return last_height;
}

void ComponentCamera::RecordFrame(GLsizei width, GLsizei height, bool scene_mode)
{

	SetWidthAndHeight(width, height);

#if !GAME
		App->renderer->anti_aliasing ? glBindFramebuffer(GL_FRAMEBUFFER, msfbo) : glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#endif
	glViewport(0, 0, width, height);

	switch (camera_clear_mode)
	{
		case ComponentCamera::ClearMode::COLOR:
			glClearColor(camera_clear_color[0], camera_clear_color[1], camera_clear_color[2], 1.f);
			glStencilMask(0xFF);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(0.f, 0.f, 0.f, 1.f);
			break;
		case ComponentCamera::ClearMode::SKYBOX:
			glStencilMask(0xFF);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			if (skybox_uuid != 0)
			{
				camera_skybox->Render(*this);
			}
			else
			{
				App->cameras->world_skybox->Render(*this);
			}
			break;
		default:
			break;
	}

	App->renderer->RenderFrame(*this);


	BROFILER_CATEGORY("Canvas", Profiler::Color::AliceBlue);
	App->ui->Render(scene_mode);

#if !GAME
	if (App->renderer->anti_aliasing)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msfbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

void ComponentCamera::RecordZBufferFrame(GLsizei width, GLsizei height)
{

	SetWidthAndHeight(width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	glViewport(0, 0, width, height);

	glClear(GL_DEPTH_BUFFER_BIT);
	App->renderer->RenderZBufferFrame(*this);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentCamera::SetWidthAndHeight(const GLsizei &width, const GLsizei &height)
{
	if (last_width != width || last_height != height || toggle_msaa)
	{
		last_width = static_cast<float>(width);
		last_height = static_cast<float>(height);
		SetAspectRatio(last_width / last_height);
		GenerateFrameBuffers(width, height);
		toggle_msaa = false;
	}
}

void ComponentCamera::RecordDebugDraws(bool scene_mode)
{
#if !GAME
	App->renderer->anti_aliasing ? glBindFramebuffer(GL_FRAMEBUFFER, msfbo) : glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#endif
	glViewport(0, 0, static_cast<GLsizei>(last_width), static_cast<GLsizei>(last_height));

	if (scene_mode)
	{
		App->debug_draw->RenderGrid();
		if (App->debug->show_navmesh)
		{
			App->debug_draw->RenderNavMesh(*this);
		}
		App->debug_draw->RenderBillboards();
		if (App->editor->selected_game_object != nullptr)
		{
			App->debug_draw->RenderOutline(); // This function tries to render again the selected game object. It will fail because depth buffer
		}
	}

	App->debug_draw->RenderDebugDraws(*this);
#if !GAME
	if (App->renderer->anti_aliasing)
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, msfbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glBlitFramebuffer(0, 0, last_width, last_height, 0, 0, last_width, last_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

GLuint ComponentCamera::GetLastRecordedFrame() const
{
	return last_recorded_frame_texture;
}

void ComponentCamera::GenerateFrameBuffers(GLsizei width, GLsizei height)
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

	if (depth_rbo != 0)
	{
		glDeleteRenderbuffers(1, &depth_rbo);
	}

	App->renderer->anti_aliasing ? CreateMssaFramebuffer(width, height) : CreateFramebuffer(width, height);
}

void ComponentCamera::CreateFramebuffer(GLsizei width, GLsizei height)
{

	if (camera_frustum.type == FrustumType::PerspectiveFrustum) //Scene and game cameras render this way
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

	if (camera_frustum.type == FrustumType::OrthographicFrustum) //Light cameras render this way
	{
		glCullFace(GL_FRONT);
		CreateOrthographicFramebuffer(width, height);
		glCullFace(GL_BACK);
	}
	
}

void ComponentCamera::CreateOrthographicFramebuffer(GLsizei width, GLsizei height)
{
	glGenRenderbuffers(1, &depth_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentCamera::CreateMssaFramebuffer(GLsizei width, GLsizei height)
{

	if (camera_frustum.type == FrustumType::PerspectiveFrustum)
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

	if (camera_frustum.type == FrustumType::OrthographicFrustum) //Light cameras render this way
	{
		CreateOrthographicFramebuffer(width, height);

	}
	
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

bool ComponentCamera::IsInsideFrustum(const AABB2D& aabb) const
{
	return CheckAABB2DCollision(aabb) != ComponentAABB::CollisionState::OUTSIDE;
}

bool ComponentCamera::IsCompletlyInsideFrustum(const AABB& aabb) const
{
	return CheckAABBCollision(aabb) == ComponentAABB::CollisionState::INSIDE;
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

