#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "Hierarchy.h"

class ComponentCamera;

class ModuleScene : public Module
{
public:
	ModuleScene() = default;
	~ModuleScene() = default;

	bool Init();
	update_status Update() override;
	bool CleanUp();

	GameObject* CreateGameObject();
	GameObject* CreateChildGameObject(GameObject *parent);
	void RemoveGameObject(GameObject * game_object_to_remove);

	GameObject* GetRoot() const;
	GameObject* GetGameObject(unsigned int UUID) const;

	void DeleteCurrentScene();
	void Save(Config& serialized_scene) const;
	void Load(const Config& serialized_scene);

	void MousePicking(const float2& mouse_position);
	void DrawGizmo(const ComponentCamera& camera, const GameObject& game_object) const;

	void ShowFrameBufferTab(ComponentCamera & camera_frame_buffer_to_show, const char * title);

public:
	Hierarchy hierarchy;
	bool scene_window_is_hovered = false;

private:
	float imgui_window_content_width = 0;
	float imgui_window_content_height = 0;

	float2 imgui_window_content_pos = float2::zero;

	GameObject *root = nullptr;
	std::vector<std::unique_ptr<GameObject>> game_objects_ownership;
};

#endif // _MODULSESCENE_H