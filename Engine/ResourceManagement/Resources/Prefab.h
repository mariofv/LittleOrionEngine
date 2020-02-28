#ifndef _PREFAB_H_
#define _PREFAB_H_

#include "Resource.h"
#include "ResourceManagement/Loaders/PrefabLoader.h"
#include <vector>
#include <memory>
class GameObject;
class Prefab : public Resource
{
public:
	Prefab(uint32_t UID, const std::string & exported_file);
	~Prefab() = default;
	void Instantiate(const GameObject * prefab_parent);

private:
	void LoadInMemory();
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



