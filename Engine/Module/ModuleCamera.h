#ifndef _MODULECAMERA_H_
#define _MODULECAMERA_H_

#include "Module.h"
#include "Globals.h"

#include "Geometry/Frustum.h"
#include "MathGeoLib.h"

class GameObject;
class ComponentCamera;

class ModuleCamera : public Module
{
public:
	ModuleCamera() = default;
	~ModuleCamera() = default;
	
	bool Init();
	update_status Update();
	bool CleanUp();
	
	ComponentCamera* CreateComponentCamera() const;

	void SetOrbit(const bool is_orbiting);
	bool IsOrbiting() const;

	void SetMovement(const bool movement_enabled);
	bool IsMovementEnabled() const;

	void ShowCameraOptions();
	
public:
	ComponentCamera *scene_camera_component = nullptr;

private:
	GameObject *scene_camera_game_object = nullptr;

	bool movement_enabled = false;

	bool is_orbiting = false;
	float speed_up;
};

#endif //_MODULECAMERA_H_