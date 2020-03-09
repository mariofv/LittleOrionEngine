#ifndef _SKELETON_H_
#define _SKELETON_H_
#include "Resource.h"
#include <Component/ComponentTransform.h> //Need transform separate from component
#include <ResourceManagement/Loaders/SkeletonLoader.h>
#include <vector>
class Skeleton : public Resource
{
public:
	struct Joint {
		math::float4x4 transform_global;
		math::float4x4 transform_local;
		uint32_t parent_index;
		std::string name;
	};

	Skeleton(const char * const UID, const std::string & exported_file);
	Skeleton(std::vector<Joint> && joints, const std::string & exported_file);
	~Skeleton() = default;

private:
	void LoadInMemory() override;

public:

	std::vector<Joint> skeleton;
};

namespace Loader
{
	template<>
	static std::shared_ptr<Skeleton> Load(const std::string& uid) {
		return SkeletonLoader::Load(uid);
	}
}
#endif //_SKELETON_H_

