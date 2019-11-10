#pragma once
#include "Module.h"
#include "Globals.h"
#include "Geometry/Frustum.h"
#include "MathGeoLib.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();
	
	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	
	void SetFOV(const float fov);
	void SetAspectRatio(const float aspect_ratio);
	void SetNearDistance(const float distance);
	void SetFarDistance(const float distance);
	void SetPosition(const float3 position);
	void SetOrientation(const float3 orientation);
	void LookAt(const float x, const float y, const float z);

	void MoveUp();
	void MoveDown();
	void MoveFoward();
	void MoveBackward();
	void MoveLeft();
	void MoveRight();

	void RotatePitch(const float angle);
	void RotateYaw(const float angle);

	void SetSpeedUp(const bool is_speeding_up);

private:
	void generateMatrices();

public:
	#define CAMERA_MOVEMENT_SPEED 0.1f
	#define CAMERA_ROTATION_SPEED 0.01f
	#define SPEED_UP_FACTOR 10

	float4x4 proj;
	float4x4 view;

private:
	Frustum camera_frustum;
	
	float aspect_ratio;
	float pitch_angle;
	float speed_up;
};
