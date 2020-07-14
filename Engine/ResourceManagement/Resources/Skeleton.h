#ifndef _SKELETON_H_
#define _SKELETON_H_

#include "Resource.h"
#include "ResourceManagement/Manager/SkeletonManager.h"

#include <MathGeoLib/MathGeoLib.h>
#include <vector>

class Metafile;


class Skeleton : public Resource
{
public:
	struct Joint 
	{
		math::float4x4 transform_global;
		math::float4x4 transform_local;
		uint32_t parent_index;
		std::string name;
	};

	Skeleton() = default;
	Skeleton(uint32_t uuid);
	Skeleton(uint32_t uuid, std::vector<Joint> && joints);
	~Skeleton() = default;

public:
	std::vector<Joint> skeleton;
};

namespace ResourceManagement
{
	template<>
	static std::shared_ptr<Skeleton> Load(uint32_t uuid, const FileData& resource_data, bool async)
	{
		return SkeletonManager::Load(uuid, resource_data);
	}
}

#endif //_SKELETON_H_

