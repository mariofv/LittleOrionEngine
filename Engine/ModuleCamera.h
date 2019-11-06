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

	void MoveUp(float distance);
	void MoveDown(float distance);

private:
	void generateMatrices();

public:
	float4x4 proj;
	float4x4 view;

private:
	Frustum camera_frustum;
	float aspect_ratio;
};
