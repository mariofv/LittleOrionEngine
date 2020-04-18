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
	STATE_MACHINE,
	UNKNOWN
};

class Resource
{
public:
	Resource() = default;
	Resource(Metafile* metafile);
	Resource& operator=(const Resource& resource_to_copy) = default;

	virtual ~Resource() = default;

	std::string GetName() const;
	uint32_t GetUUID() const;

	static std::string GetResourceTypeName(ResourceType resource_type);

public:
	Metafile* resource_metafile = nullptr;
};

namespace ResourceManagement
{
	template<typename T>
	static FileData Binarize(const T& resource)
	{
		assert(1 != 0); // If you are here, implement your Create function on the corresponding resource class.
		return NULL;
	};

	template<typename T>
	static std::shared_ptr<T> Load(Metafile* metafile, const FileData& resource_data)
	{
		return nullptr;
	};

	template<typename T>
	static FileData Create()
	{
		assert(1 != 0); // If you are here, implement your Create function on the corresponding resource class.
		return NULL;
	};
}

#endif // !_RESOURCE_H_

