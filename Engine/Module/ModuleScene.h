#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_
#define ENGINE_EXPORTS
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

	ENGINE_API GameObject* CreateGameObject();
	ENGINE_API GameObject* CreateChildGameObject(GameObject *parent);
	void RemoveGameObject(GameObject * game_object_to_remove);
	GameObject* AddGameObject(std::unique_ptr<GameObject> & game_object_to_add);

	GameObject* GetRoot() const;
	ENGINE_API GameObject* GetGameObject(uint64_t UUID) const;
	Component* GetComponent(uint64_t UUID) const;

	void DeleteCurrentScene();
	void NewScene(const std::string &path);
private:
	GameObject *root = nullptr;
	std::vector<std::unique_ptr<GameObject>> game_objects_ownership;

	friend class PanelScene;
	friend class ModuleDebugDraw;
};

#endif // _MODULSESCENE_H