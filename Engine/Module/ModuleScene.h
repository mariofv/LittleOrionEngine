#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include "Module.h"
#include "Main/Globals.h"
#include "Main/GameObject.h"

#include <ImGuizmo.h>

class ComponentCamera;

class ModuleScene : public Module
{
public:
	ModuleScene() = default;
	~ModuleScene() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	GameObject* CreateGameObject();
	GameObject* CreateChildGameObject(GameObject *parent);
	void RemoveGameObject(GameObject * game_object_to_remove);

	GameObject* GetRoot() const;
	GameObject* GetGameObject(uint64_t UUID) const;
	Component* GetComponent(uint64_t UUID) const;

	void DeleteCurrentScene();
	void Save(Config& serialized_scene) const;
	void Load(const Config& serialized_scene);

private:
	GameObject *root = nullptr;
	std::vector<std::unique_ptr<GameObject>> game_objects_ownership;

	friend class PanelScene;
	friend class ModuleDebugDraw;
};

#endif // _MODULSESCENE_H