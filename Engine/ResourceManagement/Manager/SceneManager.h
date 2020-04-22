#ifndef _SCENEMANAGER_H_
#define _SCENEMANAGER_H_

#include <string>
#include <vector>

class Config;
class GameObject;

class SceneManager
{
public:
	SceneManager() = default;
	~SceneManager() = default;

	void Save(const std::string& path, GameObject* gameobject_to_save) const;
	void Load(const std::string& path) const;

private:
	void SavePrefab(Config& config, GameObject* gameobject_to_save) const;
	void SavePrefabUUIDS(std::vector<Config>& config, GameObject* gameobject_to_save) const;
	bool SaveModifiedPrefabComponents(Config& config, GameObject* gameobject_to_save) const;
	GameObject * LoadPrefab(const Config& config) const;
	void LoadPrefabModifiedComponents(const Config& config) const;
};

#endif