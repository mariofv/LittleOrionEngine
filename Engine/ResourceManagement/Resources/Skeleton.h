#ifndef _SKELETON_H_
#define _SKELETON_H_
#include "Resource.h"
#include <Component/ComponentTransform.h>
#include <vector>
class Skeleton : public Resource
{
public:
	struct Joint {
		math::float4x4 transform;
		uint32_t parent_index;
	};

	Skeleton(const char * const UID, const std::string & exported_file);
	Skeleton(std::vector<Joint> && joints, const std::string & exported_file);
	~Skeleton() = default;

	std::vector<Joint> skeleton;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

private:
	void LoadInMemory() override;
};
#endif //_SKELETON_H_

