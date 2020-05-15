#ifndef _PREFABMANAGER_H_
#define _PREFABMANAGER_H_

#include "Filesystem/File.h"

#include <memory>
#include <string>

class Config;
class GameObject;
class Metafile;
class Prefab;

class PrefabManager
{
public:
	PrefabManager() = default;
	~PrefabManager() = default;

	static FileData Binarize(Prefab* prefab);
	static std::shared_ptr<Prefab> Load(uint32_t uuid, const FileData& resource_data);
	static uint32_t CreateFromGameObject(Path& creation_folder_path, GameObject& gameobject);

private:
	static void LoadBasicParameters(const Config& config, std::unique_ptr<GameObject> & loaded_gameObject);
	static void CreateComponents(const Config& config, std::unique_ptr<GameObject> & loaded_gameObject);

};

#endif // !_PREFABLOADER_H_

