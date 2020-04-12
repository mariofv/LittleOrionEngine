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

	static std::string GetResourceTypeName(ResourceType resource_type);

public:
	Metafile* resource_metafile = nullptr;
};

namespace Loader
{
	template<typename T>
	static std::shared_ptr<T> Load(Metafile* metafile, const FileData& resource_data)
	{
		return nullptr;
	};
}

#endif // !_RESOURCE_H_

