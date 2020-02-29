#ifndef _PREFABLOADER_H_
#define _PREFABLOADER_H_

#include <memory>
#include <string>

class Prefab;
class GameObject;
class Config;
class PrefabLoader
{
public:
	PrefabLoader() = default;
	~PrefabLoader() = default;
	static std::shared_ptr<Prefab> Load(const std::string& file_path);
private:
	static void LoadBasicParameters(const Config& config, std::unique_ptr<GameObject> & loaded_gameObject);
	static void CreateComponents(const Config& config, std::unique_ptr<GameObject> & loaded_gameObject);
};


#endif // !_PREFABLOADER_H_

