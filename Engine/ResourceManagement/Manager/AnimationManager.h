#ifndef _ANIMATIONMANAGER_H_
#define _ANIMATIONMANAGER_H_

#include <memory>
#include <string>

#include "Filesystem/File.h"

class Animation;
class Metafile;

class AnimationManager
{
public:
	AnimationManager() = default;
	~AnimationManager() = default;

	static std::shared_ptr<Animation> Load(uint32_t uuid, const FileData& resource_data);
};

#endif //_ANIMATIONMANAGER_H_
