#include "Animation.h"


Animation::Animation(const uint32_t UUID, const std::string & exported_file) : Resource(UUID, exported_file)
{

}

Animation::Animation(std::vector<KeyFrame> && keyframes, std::string name, float frames, float frames_per_second, const uint32_t UUID,const std::string & exported_file)
	: keyframes(keyframes)
	, name(name)
	, frames(frames)
	, frames_per_second(frames_per_second)
	, Resource(UUID, exported_file)
{

}
