#include "Animation.h"


Animation::Animation(const char * const UID, const std::string & exported_file) : Resource(UID, exported_file)
{

}

Animation::Animation(std::vector<Channel> && channels, std::string name, float duration, const std::string & exported_file)
	: channels(channels)
	, name(name)
	, duration(duration)
	, Resource("", exported_file)
{

}
