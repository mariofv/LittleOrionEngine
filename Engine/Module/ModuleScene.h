#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_
#define ENGINE_EXPORTS

#include "Module.h"
#include "Helper/BuildOptions.h"
#include "Main/Globals.h"
#include "Main/GameObject.h"

class Scene;

class ModuleScene : public Module
{
public:
	ModuleScene() = default;
	~ModuleScene() = default;

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	ENGINE_API GameObject* CreateGameObject();
	ENGINE_API GameObject* CreateChildGameObject(GameObject* parent);
	void RemoveGameObject(GameObject* game_object_to_remove);
	GameObject* AddGameObject(std::unique_ptr<GameObject> & game_object_to_add);

	ENGINE_API GameObject* GetRoot() const;
	ENGINE_API GameObject* GetGameObject(uint64_t UUID) const;
	Component* GetComponent(uint64_t UUID) const;


	void OpenPendingScene();
	void DeleteCurrentScene();

	ENGINE_API void LoadBuildScene(unsigned scene);
	void LoadScene(const std::string& path);
	void SaveScene(const std::string& path) const;
	bool HasPendingSceneToLoad() const;

private:
	void OpenScene(const std::string &path);
	void GetSceneFromPath(const std::string& path);

private:
	GameObject* root = nullptr;
	std::vector<std::unique_ptr<GameObject>> game_objects_ownership;
	std::shared_ptr<Scene> current_scene = nullptr;
	std::string scene_to_load;
	std::unique_ptr<BuildOptions> build_options = nullptr;

	friend class PanelScene;
	friend class PanelBuildOptions;
	friend class ModuleDebugDraw;
};

#endif // _MODULSESCENE_H