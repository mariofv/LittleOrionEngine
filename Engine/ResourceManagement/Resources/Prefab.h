#ifndef _PREFAB_H_
#define _PREFAB_H_

#include "Resource.h"
#include "ResourceManagement/Manager/PrefabManager.h"

#include "EditorUI/Panel/PanelHierarchy.h"
#include "EditorUI/Panel/PanelScene.h"

#include "Main/GameObject.h"

#include <vector>
#include <memory>
#include <unordered_map>

class Component;
class Metafile;

class Prefab : public Resource
{
public:
	Prefab() = default;
	Prefab(uint32_t uuid) : Resource(uuid) {};
	Prefab(uint32_t uuid, std::vector<std::unique_ptr<GameObject>> && gameObjects, bool overwritable);
	~Prefab();

	GameObject* Instantiate(GameObject* prefab_parent, std::unordered_map<int64_t, int64_t>* UUIDS_pairs = nullptr);
	void Duplicate(GameObject* instance);

	void Apply(GameObject * new_reference);
	void Revert(GameObject * old_reference);
	void RemoveInstance(GameObject * instance);
	bool IsOverwritable() const;

	void Reassign();

	GameObject* GetRootGameObject() const;
	GameObject* GetOriginalGameObject(int64_t UUID) const;

public:
	std::vector<GameObject*> instances;

private:
	void RecursiveRewrite(GameObject * old_instance, GameObject * new_reference, bool original, bool revert);
	void AddNewGameObjectToInstance(GameObject * old_instance, GameObject * new_reference, bool original, bool revert);
	void RemoveGameObjectFromOriginalPrefab(GameObject * gameobject_to_remove);
	std::vector<std::unique_ptr<GameObject>> prefab;
	bool overwritable = true;

	friend PanelHierarchy;
	friend PanelScene;
};

namespace ResourceManagement
{
	template<>
	static FileData Binarize<Prefab>(Resource* prefab)
	{
		return PrefabManager::Binarize(static_cast<Prefab*>(prefab));
	};

	template<>
	static std::shared_ptr<Prefab> Load(uint32_t uuid, const FileData& resource_data, bool async)
	{
		return PrefabManager::Load(uuid, resource_data);
	}
}

#endif // !_PREFAB_H_



