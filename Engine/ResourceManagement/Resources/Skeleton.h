#ifndef _SKELETON_H_
#define _SKELETON_H_

#include "Resource.h"

#include <MathGeoLib/MathGeoLib.h>
#include <vector>

class Metafile;

class Skeleton : public Resource
{
public:
	struct Joint {
		math::float4x4 transform_global;
		math::float4x4 transform_local;
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

#endif //_SKELETON_H_

