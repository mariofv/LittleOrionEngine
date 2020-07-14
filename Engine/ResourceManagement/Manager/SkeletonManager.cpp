#include "SkeletonManager.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

#include "ResourceManagement/Metafile/Metafile.h"
#include "ResourceManagement/Resources/Skeleton.h"

#include <Brofiler/Brofiler.h>
#include <vector>

std::shared_ptr<Skeleton> SkeletonManager::Load(uint32_t uuid, const FileData& resource_data)
{
	char* data = (char*) resource_data.buffer;
	char* cursor = data;

	uint32_t num_bones;
	//Get number of bones
	size_t bytes = sizeof(uint32_t);
	memcpy(&num_bones, cursor, bytes);

	std::vector<Skeleton::Joint> bones;

	bones.resize(num_bones);
	cursor += bytes;
	for (auto & joint : bones)
	{
		uint32_t name_size;
		memcpy(&name_size, cursor, sizeof(uint32_t));
		cursor += sizeof(uint32_t);
		joint.name.resize(name_size);
		memcpy(&joint.name[0], cursor, name_size);
		cursor += name_size;
		memcpy(&joint.transform_global, cursor, sizeof(float4x4));
		cursor += sizeof(float4x4);
		memcpy(&joint.transform_local, cursor, sizeof(float4x4));
		cursor += sizeof(float4x4);
		memcpy(&joint.parent_index, cursor,sizeof(uint32_t));
		cursor += sizeof(uint32_t);
	}

	std::shared_ptr<Skeleton> new_skeleton = std::make_shared<Skeleton>(uuid, std::move(bones));
	return new_skeleton;
}