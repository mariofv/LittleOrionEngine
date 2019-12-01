#ifndef _COMPONENTCAMERA_H_
#define _COMPONENTCAMERA_H_

#include "Component.h"

#include "MathGeoLib.h"
#include <GL/glew.h>

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject * owner);

	~ComponentCamera() = default;

	void Enable() override;
	void Disable() override;
	void Update() override;

	void SetFOV(const float fov);
	void SetAspectRatio(const float aspect_ratio);
	void SetNearDistance(const float distance);
	void SetFarDistance(const float distance);
	void SetOrientation(const float3 orientation);
	void SetOrthographicSize(const float2 size);
	void LookAt(const float3 focus);
	void LookAt(const float x, const float y, const float z);

	void RotatePitch(const float angle);
	void RotateYaw(const float angle);

	void SetPerpesctiveView();
	void SetOrthographicView();

	void ShowComponentWindow() override;

private:
	void GenerateMatrices();

public:
	float camera_movement_speed = 1.0f;
	float camera_zooming_speed = 1.0f;
	float camera_rotation_speed = 0.000625f;

	float4x4 proj;
	float4x4 view;

private:
	Frustum camera_frustum;

	float aspect_ratio = 1.f;
	float orthographic_fov_ratio = 3;
	int perpesctive_enable = 0;
};

#endif //_COMPONENTCAMERA_H_