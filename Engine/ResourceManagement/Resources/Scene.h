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
	Scene(Metafile* resource_metafile) : Resource(resource_metafile) {};
	~Scene() = default;

	void Save(Config& config) const;
	void Load(const Config& config);


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
