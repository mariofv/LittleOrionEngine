#ifndef  __SCENECAMERASCONTROLLER_H__
#define  __SCENECAMERASCONTROLLER_H__

#include "Script.h"
class ComponentCamera;
class DebugModeScript;

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
	void SetMainCameraRendering();

	void InitPublicGameObjects();


private:
	GameObject* main_camera = nullptr;
	GameObject* camera_1 = nullptr;
	GameObject* camera_2 = nullptr;
	GameObject* camera_3 = nullptr;
	
	ComponentCamera* camera_rendering;
	std::vector<GameObject*> camera_list;
	unsigned int index = 0;

	GameObject* debug = nullptr;
	DebugModeScript* debug_mode = nullptr;
};
extern "C" SCRIPT_API SceneCamerasController* SceneCamerasControllerDLL(); //This is how we are going to load the script
#endif