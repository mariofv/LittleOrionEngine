#ifndef  __CAMERACONTROLLER_H__
#define  __CAMERACONTROLLER_H__

#include "Script.h"

#include "MathGeoLib.h"

class ComponentCamera;
class DebugModeScript;
class PlayerController;
class WorldManager;

class CameraController : public Script
{
public:
	CameraController();
	~CameraController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void ActivePlayer();
	void Focus(float3 position_to_focus);
	void FollowPlayer();

	void MultiplayerCamera();
	//Useless on future iterations because of the mechanic of "Don't die alone"
	void MultiplayerToSingleplayer();

	void SetFreeze();


	void InitPublicGameObjects();
	void SetPosition(float3 position);
	bool god_mode = false;
	bool freeze = false;

private:
	Quat rotation = Quat::FromEulerXYZ(2.742f, 0.f, -3.142f);
	float rotation_speed = 1.f;

	GameObject* player1 = nullptr;
	PlayerController* player_movement_script = nullptr;
	ComponentScript* player_movement_component = nullptr;

	GameObject* player2 = nullptr;
	PlayerController* player2_movement_script = nullptr;
	ComponentScript* player2_movement_component = nullptr;
	WorldManager* world_manager = nullptr;

	const float CENTER_TIME = 1000000.f;

	float3 offset_near = float3(0.f, 5.5f, 11.f);
	float3 offset_far = float3(0.f, 8.5f, 20.5f);
	float3 selected_offset = float3::zero;;

	bool is_focusing = false;
	float start_focus_time = 0.f;
	float current_time = 0.f;

	bool multiplayer = false;
	float distance_x = 0.f;
	float distance_z = 0.f;


	GameObject* current_target = nullptr;

};
extern "C" SCRIPT_API CameraController* CameraControllerDLL(); //This is how we are going to load the script
#endif