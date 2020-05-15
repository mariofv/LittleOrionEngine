#ifndef  __GODCAMERA_H__
#define  __GODCAMERA_H__

#include "Script.h"

class CameraController;
class ComponentCamera;
class DebugModeScript;

class GodCamera : public Script
{
public:
	GodCamera();
	~GodCamera() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void GodModeCamera();

	void Active();

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;

private:
	float rotation_speed = 1.f;
	bool god_mode = false;
	ComponentCamera* camera_component = nullptr;

	GameObject* game_camera = nullptr;
	ComponentCamera* game_camera_component = nullptr;
	CameraController* camera_controller = nullptr;

	GameObject* debug = nullptr;
	DebugModeScript* debug_mode = nullptr;

};
extern "C" SCRIPT_API GodCamera* GodCameraDLL(); //This is how we are going to load the script
#endif