#ifndef _SCENE_H_
#define _SCENE_H_

#include "Resource.h"
#include "ResourceManagement/Manager/SceneManager.h"

#include <array>
#include <vector>

class Config;
class Metafile;

class Scene : public Resource
{

public:
	Scene(Metafile* resource_metafile, const Config& config);
	~Scene() = default;

	void Save(GameObject* gameobject_to_save) const;
	void Load();

private:
	void SavePrefab(Config& config, GameObject* gameobject_to_save) const;
	void SavePrefabUUIDS(std::vector<Config>& config, GameObject* gameobject_to_save) const;
	bool SaveModifiedPrefabComponents(Config& config, GameObject* gameobject_to_save) const;
	GameObject * LoadPrefab(const Config& config) const;
	void LoadPrefabModifiedComponents(const Config& config) const;

private:
	std::unique_ptr<Config> scene_config = nullptr;

};


namespace Loader
{
	template<>
	static std::shared_ptr<Scene> Load(Metafile* metafile, const FileData& resource_data)
	{
		return SceneManager::Load(metafile, resource_data);
	}
}

#endif //_SCENE_H_
