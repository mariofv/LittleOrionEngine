#ifndef  __CAMERACONTROLLER_H__
#define  __CAMERACONTROLLER_H__

#include "Script.h"
class TestScriptRuntime;
class ComponentCamera;

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

	void FollowPlayer();

	void Save(Config& config) const override;
	void Load(const Config& config) override;
	void Link() override;

private:
	PanelComponent* panel = nullptr;
	bool god_mode = false;

	GameObject* camera = nullptr;
	uint64_t cameraUUID = -1;
	std::string is_camera = "None";
	ComponentCamera* camera_component = nullptr;

	GameObject* player = nullptr;
	uint64_t playerUUID = -1;
	std::string is_player = "None";
	TestScriptRuntime* player_movement_script = nullptr;
	ComponentScript* player_movement_component = nullptr;

};
extern "C" SCRIPT_API CameraController* CameraControllerDLL(); //This is how we are going to load the script
#endif