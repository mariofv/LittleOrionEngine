#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "Module.h"
#include "Helper/BuildOptions.h"
#include "Main/Globals.h"
#include "Main/GameObject.h"
#include <thread>
#include <atomic>
#include <mutex>

class Scene;

class ModuleScene : public Module
{
public:
	ModuleScene() = default;
	~ModuleScene() = default;

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	ENGINE_API GameObject* CreateGameObject();
	ENGINE_API GameObject* CreateChildGameObject(GameObject* parent);
	void RemoveGameObject(GameObject* game_object_to_remove);
	GameObject* AddGameObject(std::unique_ptr<GameObject> & game_object_to_add);
	ENGINE_API GameObject* DuplicateGameObject(GameObject* game_object, GameObject* parent_go);
	void DuplicateGameObjectList(std::vector<GameObject*> game_objects);
	bool HasParentInList(GameObject* go, std::vector<GameObject*>) const;
	bool BelongsToList(GameObject* go, std::vector<GameObject*>) const;
	void InitDuplicatedScripts(GameObject* clone_go);

	ENGINE_API GameObject* GetRoot() const;
	ENGINE_API GameObject* GetGameObject(uint64_t UUID) const;
	ENGINE_API GameObject* GetGameObjectByName(const std::string& go_name) const;

	ENGINE_API GameObject* GetGameObjectWithTag(const std::string& tag) const;
	ENGINE_API std::vector<GameObject*> GetGameObjectsWithTag(const std::string& tag) const;
	ENGINE_API std::vector<GameObject*> GetGameObjectsWithComponent(const Component::ComponentType component_type) const;

	Component* GetComponent(uint64_t UUID) const;
	void SortGameObjectChilds(GameObject* go) const;

	void OpenPendingScene();
	void DeleteCurrentScene();


	ENGINE_API void LoadScene(const std::string& path);
	ENGINE_API void LoadScene(unsigned position);

	void SaveScene(uint32_t scene_uuid = 0);

	void OpenNewScene();
	void LoadTmpScene();
	void SaveTmpScene();

	void LoadLoadingScreen();
	void DeleteLoadingScreen();

	bool HasPendingSceneToLoad() const;
	bool CurrentSceneIsSaved() const;

	void StopSceneTimer();

	//Multiselection
	bool HasParent(GameObject* go) const;
	bool BelongsToList(GameObject* go) const;

private:
	void OpenScene();
	inline void LoadSceneResource();
	uint32_t GetSceneUUIDFromPath(const std::string& path);

	//Don't use this function use the public one
	GameObject* DuplicateGO(GameObject* game_object, GameObject* parent_go);


private:
	GameObject* root = nullptr;
	std::vector<std::unique_ptr<GameObject>> game_objects_ownership;

	std::shared_ptr<Scene> current_scene = nullptr;
	uint32_t pending_scene_uuid = 0;

	std::shared_ptr<Scene> tmp_scene = nullptr;
	std::shared_ptr<Scene> last_scene = 0;

	std::unique_ptr<BuildOptions> build_options = nullptr;

	GameObject* loading_screen_canvas = nullptr;

	Timer timer;

	friend class PanelScene;
	friend class PanelBuildOptions;
	friend class ModuleDebugDraw;
	friend class PanelPopupSceneSaver;
};

#endif // _MODULSESCENE_H