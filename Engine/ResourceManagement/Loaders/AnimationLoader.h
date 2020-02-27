#ifndef _ANIMATIONLOADER_H_
#define _ANIMATIONLOADER_H_

#include <memory>
#include <string>
class Animation;
class AnimationLoader
{
public:
	AnimationLoader() = default;
	~AnimationLoader() = default;


	static std::shared_ptr<Animation> Load(const std::string& uid);
};

#endif
