#include "Animation.h"


Animation::Animation(const uint32_t UID, const std::string & exported_file) : Resource(UID, exported_file)
{

}

Animation::Animation(std::vector<KeyFrame> && keyframes, std::string name, float duration, const std::string & exported_file)
	: keyframes(keyframes)
	, name(name)
	, duration(duration)
	, Resource(0, exported_file)
{

}
