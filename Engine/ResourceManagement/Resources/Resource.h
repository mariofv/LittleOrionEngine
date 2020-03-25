#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Filesystem/File.h"

#include <memory>
#include <string>

class Path;
class Metafile;

enum class ResourceType
{
	ANIMATION,
	AUDIO,
	MATERIAL,
	MODEL,
	MESH,
	NAVMESH,
	PREFAB,
	SCENE,
	SKELETON,
	SKYBOX,
	TEXTURE,
	UNKNOWN
};

class Resource
{
public:
	Resource() = default;
	Resource(Metafile* metafile);
	virtual ~Resource() = default;

	uint32_t GetUUID() const;

public:
	Metafile* resource_metafile = nullptr;
};

namespace ResourceManager
{
	template<typename T>
	uint32_t Create(const Path& asset_creation_folder_path, const std::string created_asset_name="")
	{
		return -1;
	};
}
#endif // !_RESOURCE_H_

