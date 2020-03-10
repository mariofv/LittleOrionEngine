#ifndef _SKELERONLOADER_H_
#define _SKELERONLOADER_H_
#include <memory>
#include <string>
class Skeleton;
class SkeletonLoader
{
public:
	SkeletonLoader() = default;
	~SkeletonLoader() = default;

	static std::shared_ptr<Skeleton> Load(const std::string& uid);
};
#endif // _SKELERONLOADER_H_
