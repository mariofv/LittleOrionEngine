#ifndef _PREFAB_H_
#define _PREFAB_H_

#include "Resource.h"
#include "EditorUI/Panel/PanelHierarchy.h"
#include "EditorUI/Panel/PanelScene.h"

#include <vector>
#include <memory>
#include <unordered_map>

class Component;
class GameObject;
class Metafile;

class Prefab : public Resource
{
public:
	Prefab() = default;
	Prefab(Metafile* resource_metafile) : Resource(resource_metafile) {};
	Prefab(Metafile* resource_metafile, std::vector<std::unique_ptr<GameObject>> && gameObjects);
	~Prefab() = default;

	GameObject * Instantiate(GameObject * prefab_parent, std::unordered_map<int64_t, int64_t> * UUIDS_pairs = nullptr);
	void Apply(GameObject * new_reference);
	void Revert(GameObject * old_reference);
	void RemoveInstance(GameObject * instance);
	bool IsOverwritable() const;

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

#endif // !_PREFAB_H_



