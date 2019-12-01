#ifndef _MODULECAMERA_H_
#define _MODULECAMERA_H_

#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

#include "Geometry/Frustum.h"
#include "MathGeoLib.h"


class ComponentCamera;
class GameObject;

class ModuleCamera : public Module
{
public:
	ModuleCamera() = default;
	~ModuleCamera() = default;
	
	bool Init();
	update_status Update();
	bool CleanUp();
	
	ComponentCamera* CreateComponentCamera();
	void RemoveComponentCamera(ComponentCamera* camera_to_remove);

	void SetOrbit(const bool is_orbiting);
	bool IsOrbiting() const;

	void SetMovement(const bool movement_enabled);
	bool IsMovementEnabled() const;

	void ShowCameraOptions();
	
public:
	ComponentCamera *scene_camera = nullptr;
	ComponentCamera* active_camera = nullptr;

private:
	GameObject *scene_camera_game_object = nullptr;

	bool movement_enabled = false;
	bool game_window_is_hovered = false;

	bool is_orbiting = false;
	float speed_up;

	std::vector<ComponentCamera*> cameras;

};

#endif //_MODULECAMERA_H_