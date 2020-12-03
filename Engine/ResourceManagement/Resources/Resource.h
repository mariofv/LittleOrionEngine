#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "Filesystem/File.h"

#include <memory>
#include <string>

class Path;
class Metafile;
struct TextureLoadData;

enum class ResourceType
{
	ANIMATION = 0,
	MATERIAL = 2,
	MODEL = 3,
	MESH = 4,
	NAVMESH = 5,
	PREFAB = 6,
	SCENE = 7,
	SKELETON = 8,
	SKYBOX = 9,
	TEXTURE = 10,
	STATE_MACHINE = 11,
	FONT = 12,
	SOUND = 1,
	VIDEO = 14,
	UNKNOWN = 13
};

class Resource
{
public:
	Resource() = default;
	Resource(uint32_t uuid);
	Resource& operator=(const Resource& resource_to_copy) = default;

	virtual ~Resource() = default;

	uint32_t GetUUID() const;

	bool IsCoreResource() const;

	static std::string GetResourceTypeName(ResourceType resource_type);

public:
	bool initialized = false;

protected:
	uint32_t uuid = 0;

};

namespace ResourceManagement
{
	template<typename T>
	static FileData Binarize(Resource* resource)
	{
		assert(1 != 0); // If you are here, implement your Binarize function on the corresponding resource class.
		return NULL;
	};

	template<typename T>
	static std::shared_ptr<T> Load(uint32_t uuid, const FileData& resource_data, bool async = false)
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