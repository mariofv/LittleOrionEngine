#include "Animation.h"


Animation::Animation(const uint32_t UID, const std::string & exported_file) : Resource(UID, exported_file)
{

}

Animation::Animation(std::vector<KeyFrame> && keyframes, std::string name, float frames, float frames_per_second,const std::string & exported_file)
	: keyframes(keyframes)
	, name(name)
	, frames(frames)
	, frames_per_second(frames_per_second)
	, Resource(0, exported_file)
{

}
