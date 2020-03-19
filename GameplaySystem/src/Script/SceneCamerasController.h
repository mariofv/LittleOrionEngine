#ifndef  __SCENECAMERASCONTROLLER_H__
#define  __SCENECAMERASCONTROLLER_H__

#include "Script.h"

class SceneCamerasController : public Script
{
public:
	SceneCamerasController();
	~SceneCamerasController() = default;

	void Awake() override;
	void Start() override;
	void Update() override;

	void OnInspector(ImGuiContext*) override;

	void Save(Config& config) const override;
	void Load(const Config& config) override;
	void Link() override;

private:
	PanelComponent* panel = nullptr;
	GameObject* example = nullptr;
	uint64_t exampleUUID = -1;

};
extern "C" SCRIPT_API SceneCamerasController* SceneCamerasControllerDLL(); //This is how we are going to load the script
#endif