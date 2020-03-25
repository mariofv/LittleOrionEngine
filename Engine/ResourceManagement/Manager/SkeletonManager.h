#ifndef _SKELETONMANAGER_H_
#define _SKELETONMANAGER_H_

#include "Filesystem/File.h"

#include <memory>
#include <string>

class Metafile;
class Skeleton;

class SkeletonManager
{
public:
	SkeletonManager() = default;
	~SkeletonManager() = default;

	static std::shared_ptr<Skeleton> Load(Metafile* metafile, const FileData& resource_data);
};
#endif // _SKELETONMANAGER_H_
