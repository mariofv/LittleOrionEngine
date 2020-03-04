#ifndef _PREFAB_H_
#define _PREFAB_H_

#include "Resource.h"
#include "ResourceManagement/Loaders/PrefabLoader.h"
#include <vector>
#include <memory>
class GameObject;
class Component;
class Prefab : public Resource
{
public:
	Prefab(std::vector<std::unique_ptr<GameObject>> && gameObjects, uint32_t UID, const std::string & exported_file);
	~Prefab() = default;
	void Instantiate(GameObject * prefab_parent);
	void Rewrite(GameObject * new_reference);

	std::vector<GameObject*> instances;
private:
	void LoadInMemory() override{};
	void RecursiveRewrite(GameObject * old_instance, GameObject * new_reference, bool original);
	void AddNewGameObjectToInstance(GameObject * old_instance, GameObject * new_reference, bool original);
	void RemoveGameObjectFromOriginalPrefab(GameObject * gameobject_to_remove);
	std::vector<std::unique_ptr<GameObject>> prefab;

};

namespace Loader
{
	template<>
	static std::shared_ptr<Prefab> Load(const std::string& uid) {
		return PrefabLoader::Load(uid);
	}
}

#endif // !_PREFAB_H_



