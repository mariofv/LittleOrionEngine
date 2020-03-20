#ifndef  __SCENECAMERASCONTROLLER_H__
#define  __SCENECAMERASCONTROLLER_H__

#include "Script.h"
class ComponentCamera;

class SceneCamerasController : public Script
{
public:
	SceneCamerasController();
	~SceneCamerasController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void UpdateCameraRendering();

	void Save(Config& config) const override;
	void Load(const Config& config) override;
	void Link() override;

private:
	PanelComponent* panel = nullptr;
	GameObject* example = nullptr;
	uint64_t exampleUUID = -1;

	GameObject* main_camera = nullptr;
	uint64_t maincameraUUID = -1;
	std::string is_main_camera = "None";

	GameObject* camera_1 = nullptr;
	uint64_t camera1UUID = -1;
	std::string is_camera_1 = "None";

	GameObject* camera_2 = nullptr;
	uint64_t camera2UUID = -1;
	std::string is_camera_2 = "None";

	GameObject* camera_3 = nullptr;
	uint64_t camera3UUID = -1;
	std::string is_camera_3 = "None";
	
	ComponentCamera* camera_rendering;
	std::vector<GameObject*> camera_list;
	int index = 0;

};
extern "C" SCRIPT_API SceneCamerasController* SceneCamerasControllerDLL(); //This is how we are going to load the script
#endif