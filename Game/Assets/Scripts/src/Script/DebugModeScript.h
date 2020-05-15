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

	void OnInspector(ImGuiContext*) override;
	void InitPublicGameObjects();
	//void Save(Config& config) const override;
	//void Load(const Config& config) override;
	bool debug_enabled = false;

private:
	GameObject* background_go = nullptr;
	GameObject* text_tris_go = nullptr;
	GameObject* text_verts_go = nullptr;
	GameObject* text_fps_go = nullptr;
	ComponentImage* background = nullptr;
	ComponentText* text_tris = nullptr;
	ComponentText* text_verts = nullptr;
	ComponentText* text_fps = nullptr;

	std::string base_str_tris = "#Tris: ";
	std::string base_str_verts = "#Verts: ";
	std::string base_str_fps = "FPS: ";

	GameObject* camera_manager = nullptr;
	SceneCamerasController* scene_cameras = nullptr;


};
extern "C" SCRIPT_API DebugModeScript* DebugModeScriptDLL(); //This is how we are going to load the script
#endif