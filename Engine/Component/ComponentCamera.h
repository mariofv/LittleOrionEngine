#ifndef _COMPONENTCAMERA_H_
#define _COMPONENTCAMERA_H_

#include "Component.h"
#include "Component/ComponentAABB.h"
#include "UI/Panel/InspectorSubpanel/PanelComponent.h"
#include "UI/Panel/PanelScene.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class EditorActionModifyCamera;

class ComponentCamera : public Component
{
public:
	enum class ClearMode
	{
		COLOR = 0,
		SKYBOX = 1
	};

	ComponentCamera();
	ComponentCamera(GameObject * owner);

	~ComponentCamera();

	void Update() override;
	void Delete() override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	float GetWidth() const;
	float GetHeigt() const;

	void RecordFrame(float width, float height);
	void RecordDebugDraws(float width, float height) const;
	GLuint GetLastRecordedFrame() const;

	void SetFOV(float fov);
	void SetAspectRatio(float aspect_ratio);
	void SetNearDistance(float distance);
	void SetFarDistance(float distance);
	void SetOrientation(const float3 & orientation);
	void AlignOrientationWithAxis();
	void SetOrthographicSize(const float2 & size);
	void LookAt(const float3 & focus);
	void LookAt(float x, float y, float z);

	void SetPosition(const float3 & position);
	void MoveUp();
	void MoveDown();
	void MoveFoward();
	void MoveBackward();
	void MoveLeft();
	void MoveRight();

	void Center(const AABB &bounding_box);

	void OrbitCameraWithMouseMotion(const float2 &motion, const float3& focus_point);
	void OrbitX(float angle, const float3& focus_point);
	void OrbitY(float angle, const float3& focus_point);

	void RotateCameraWithMouseMotion(const float2 &motion);
	void RotatePitch(float angle);
	void RotateYaw(float angle);

	void SetPerpesctiveView();
	void SetOrthographicView();

	void SetClearMode(ComponentCamera::ClearMode clear_mode);

	void SetSpeedUp(bool is_speeding_up);

	void SetViewMatrix(const float4x4& view_matrix);
	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;
	float4x4 GetInverseClipMatrix() const;

	std::vector<float> GetFrustumVertices() const;
	
	bool IsInsideFrustum(const AABB& aabb) const;
	ComponentAABB::CollisionState CheckAABBCollision(const AABB& reference_AABB) const;

	bool IsInsideFrustum(const AABB2D& aabb) const;
	ComponentAABB::CollisionState CheckAABB2DCollision(const AABB2D& reference_AABB) const;

	void GetRay(const float2& normalized_position, LineSegment &return_value) const;

	AABB GetMinimalEnclosingAABB() const;

private:
	void GenerateFrameBuffers(float width, float height);
	void GenerateMatrices();
	void InitCamera();
	void CreateFramebuffer(float width, float height);
	void CreateMssaFramebuffer(float width, float height);

public:
	const float SPEED_UP_FACTOR = 2.f;
	const float BOUNDING_BOX_DISTANCE_FACTOR = 3.f;
	
	float camera_movement_speed = 0.15f;
	const float CAMERA_MAXIMUN_MOVEMENT_SPEED = 1.0f;
	const float CAMERA_MINIMUN_MOVEMENT_SPEED = 0.005f;

	const float CAMERA_ROTATION_SPEED = 0.000625f;

	float camera_clear_color[3] = {0.0f, 0.0f, 0.0f};

	int depth = 0;

	float4x4 proj;
	float4x4 view;

	bool toggle_msaa = false;

private:
	Frustum camera_frustum;

	GLuint rbo = 0;
	GLuint fbo = 0;
	GLuint msfbo = 0;
	GLuint msfb_color = 0;
	GLuint last_recorded_frame_texture = 0;

	float last_height = 0;
	float last_width = 0;

	float aspect_ratio = 1.f;
	float orthographic_fov_ratio = 3;
	int perpesctive_enable = 0;

	bool is_speeding_up = false;
	float speed_up = 1.f;

	bool is_focusing = false;
	const float CENTER_TIME = 250.f;
	float start_focus_time = 0.f;
	float3 start_focus_position = float3::zero;
	float3 goal_focus_position = float3::zero;

	ClearMode camera_clear_mode = ClearMode::COLOR;

	friend class EditorActionModifyCamera;
	friend class ModuleDebugDraw;
	friend class PanelComponent;
	friend class PanelScene;
};

#endif //_COMPONENTCAMERA_H_