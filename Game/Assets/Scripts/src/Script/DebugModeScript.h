#ifndef  __DEBUGMODESCRIPT_H__
#define  __DEBUGMODESCRIPT_H__

#include "Script.h"

class ComponentImage;
class ComponentText;
class SceneCamerasController;

class DebugModeScript : public Script
{
public:
	DebugModeScript();
	~DebugModeScript() = default;

	void Awake() override;
	void Start() override;
	void Update() override;
	void UpdateImGui(ImGuiContext*) override;

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;
	bool debug_enabled = false;
	bool render_wireframe = false;
	bool render_AABB = false;

private:
	std::string base_str_tris = "#Tris: ";
	std::string base_str_verts = "#Verts: ";
	std::string base_str_fps = "FPS: ";

	GameObject* camera_manager = nullptr;
	SceneCamerasController* scene_cameras = nullptr;
};
extern "C" SCRIPT_API DebugModeScript* DebugModeScriptDLL(); //This is how we are going to load the script
#endif