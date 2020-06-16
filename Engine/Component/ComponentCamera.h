#ifndef _COMPONENTCAMERA_H_
#define _COMPONENTCAMERA_H_

#define ENGINE_EXPORTS

#include "Component.h"
#include "Component/ComponentAABB.h"
#include "EditorUI/Panel/InspectorSubpanel/PanelComponent.h"
#include "EditorUI/Panel/PanelGame.h"
#include "EditorUI/Panel/PanelScene.h"
#include "ResourceManagement/Resources/Skybox.h"

#include <MathGeoLib.h>
#include <GL/glew.h>

class GameObject;
class EditorActionModifyCamera;

class ComponentCamera : public Component
{
public:
	enum class ClearMode
	{
		COLOR = 0,
		SKYBOX = 1,
		ORTHO = 2
	};

	ComponentCamera();
	ComponentCamera(GameObject * owner);

	~ComponentCamera();

	//Copy and move
	ComponentCamera(const ComponentCamera& component_to_copy) = default;
	ComponentCamera(ComponentCamera&& component_to_move) = default;

	ComponentCamera & operator=(const ComponentCamera & component_to_copy);
	ComponentCamera & operator=(ComponentCamera && component_to_move) = default;

	void Update() override;
	void Delete() override;

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config& config) override;
	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	float GetWidth() const;
	float GetHeight() const;

	void RecordFrame(float width, float height, bool scene_mode = false);
	ENGINE_API void RecordDebugDraws(bool scene_mode = false);
	GLuint GetLastRecordedFrame() const;

	void SetFOV(float fov);
	void SetAspectRatio(float aspect_ratio);
	void SetNearDistance(float distance);
	void SetFarDistance(float distance);
	void SetOrientation(const float3 & orientation);
	ENGINE_API void SetStartFocusPosition(const float3& focus_position);
	ENGINE_API void SetGoalFocusPosition(const float3& focus_position);
	ENGINE_API void SetFocusTime(const float focus_time);
	ENGINE_API Frustum GetFrustum();

	void AlignOrientationWithAxis();
	ENGINE_API void SetOrthographicSize(const float2 & size);
	void LookAt(const float3 & focus);
	void LookAt(float x, float y, float z);

	ENGINE_API void SetPosition(const float3 & position);
	ENGINE_API void MoveUp();
	ENGINE_API void MoveDown();
	ENGINE_API void MoveForward();
	ENGINE_API void MoveBackward();
	ENGINE_API void MoveLeft();
	ENGINE_API void MoveRight();

	ENGINE_API void Center(const AABB &bounding_box);
	ENGINE_API void CenterGame(const GameObject* go);

	void OrbitCameraWithMouseMotion(const float2 &motion, const float3& focus_point);
	void OrbitX(float angle, const float3& focus_point);
	void OrbitY(float angle, const float3& focus_point);

	void RotateCameraWithMouseMotion(const float2 &motion);
	ENGINE_API void RotatePitch(float angle);
	ENGINE_API void RotateYaw(float angle);

	void SetPerpesctiveView();
	ENGINE_API void SetOrthographicView();

	void SetClearMode(ComponentCamera::ClearMode clear_mode);
	void SetSkybox(uint32_t skybox_uuid);

	void SetSpeedUp(bool is_speeding_up);

	void SetViewMatrix(const float4x4& view_matrix);
	float4x4 GetViewMatrix() const;
	float4x4 GetProjectionMatrix() const;
	ENGINE_API float4x4 GetClipMatrix() const;
	float4x4 GetInverseClipMatrix() const;

	std::vector<float> GetFrustumVertices() const;
	
	ENGINE_API bool IsInsideFrustum(const AABB& aabb) const;
	ComponentAABB::CollisionState CheckAABBCollision(const AABB& reference_AABB) const;

	ENGINE_API bool IsInsideFrustum(const AABB2D& aabb) const;
	ENGINE_API bool IsCompletlyInsideFrustum(const AABB & aabb) const;
	ComponentAABB::CollisionState CheckAABB2DCollision(const AABB2D& reference_AABB) const;

	ENGINE_API void GetRay(const float2 &mouse_position, LineSegment &return_value) const;

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
	bool is_focusing = false;

	Frustum camera_frustum;

private:
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
	const float CENTER_TIME = 250.f;
	float start_focus_time = 0.f;
	float3 start_focus_position = float3::zero;
	float3 goal_focus_position = float3::zero;

	ClearMode camera_clear_mode = ClearMode::SKYBOX;

	uint32_t skybox_uuid = 0;
	std::shared_ptr<Skybox> camera_skybox = nullptr;

	friend class EditorActionModifyCamera;
	friend class ModuleDebugDraw;
	friend class PanelComponent;
	friend class PanelScene;
	friend class ModuleRender;
};

#endif //_COMPONENTCAMERA_H_
