#ifndef  __DEBUGMODESCRIPT_H__
#define  __DEBUGMODESCRIPT_H__

#include "Script.h"
#include "Module/ModuleInput.h"
#include "Component/ComponentCamera.h"
#include "Module/ModuleCamera.h"

class ComponentImage;
class ComponentText;
class SceneCamerasController;
class EnemyManager;

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
	bool render_wireframe = false;
	bool is_player_invincible = false;
	bool is_warping_player_one = false;
	bool is_warping_player_two = false;

	GameObject* enemy_manager_obj;
	GameObject* player_one_obj;
	GameObject* player_two_obj;
private:
	std::string base_str_tris = "#Tris: ";
	std::string base_str_verts = "#Verts: ";
	std::string base_str_fps = "FPS: ";

	GameObject* camera_manager = nullptr;
	SceneCamerasController* scene_cameras = nullptr;
	EnemyManager* enemy_manager;
	bool has_warped_player_recently = false;
	float warp_cooldown = 0.0f;
	int selected_spawner = 0;
	bool is_spawning_enemy = false;
	const char* last_touched_game_object = "";

	bool show_aabbtree = false;
	bool show_navmesh = false;
	bool show_quadtree = false;
	bool show_octtree = false;
	bool show_bounding_boxes = false;
	bool show_global_bounding_boxes = false;
	bool show_pathfind_points = false;
	bool show_physics = false;
};
extern "C" SCRIPT_API DebugModeScript* DebugModeScriptDLL(); //This is how we are going to load the script
#endif