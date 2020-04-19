#ifndef  __CAMERACONTROLLER_H__
#define  __CAMERACONTROLLER_H__

#include "Script.h"

#include "MathGeoLib.h"

class ComponentCamera;
class DebugModeScript;
class PlayerController;

class CameraController : public Script
{
public:
	CameraController();
	~CameraController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void GodCamera();
	void ActivePlayer();
	void FollowPlayer();


	void InitPublicGameObjects();

private:
	bool god_mode = false;
	Quat rotation = Quat::identity;
	GameObject* camera = nullptr;
	ComponentCamera* camera_component = nullptr;
	float rotation_speed = 1.f;

	GameObject* player = nullptr;
	PlayerController* player_movement_script = nullptr;
	ComponentScript* player_movement_component = nullptr;

	GameObject* debug = nullptr;
	DebugModeScript* debug_mode = nullptr;


};
extern "C" SCRIPT_API CameraController* CameraControllerDLL(); //This is how we are going to load the script
#endif