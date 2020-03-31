#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <memory>
#include <string>

enum class ResourceType
{
	ANIMATION,
	AUDIO,
	MATERIAL,
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
	Resource(uint32_t UUID, const std::string & exported_file) : UUID(UUID), exported_file(exported_file) {
		int x = 0;
	};
	virtual ~Resource() = default;

	uint32_t GetUUID() const { return UUID; };
private:
	virtual void LoadInMemory() = 0;

public:
	std::string exported_file;
	std::string imported_file;

private:
	const uint32_t UUID;

};

namespace Loader
{
	template<typename T>
	static std::shared_ptr<T> Load(const std::string& uid) {
		return nullptr;
	};
}
#endif // !_RESOURCE_H_

