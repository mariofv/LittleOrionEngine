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
		uint32_t parent_index;
		std::string name;
	};

	Skeleton() = default;
	Skeleton(Metafile* resource_metafile);
	Skeleton(Metafile* resource_metafile, std::vector<Joint> && joints);
	~Skeleton() = default;

public:
	std::vector<Joint> skeleton;
};

namespace Loader
{
	template<>
	static std::shared_ptr<Skeleton> Load(Metafile* metafile, const FileData& resource_data)
	{
		return SkeletonManager::Load(metafile, resource_data);
	}
}

#endif //_SKELETON_H_

