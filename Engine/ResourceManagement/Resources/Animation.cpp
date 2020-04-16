#include "Animation.h"

#include "ResourceManagement/Metafile/Metafile.h"

Animation::Animation(Metafile* resource_metafile, std::vector<KeyFrame> && keyframes, std::string name, float frames, float frames_per_second)

	: keyframes(keyframes)
	, name(name)
	, frames(frames)
	, frames_per_second(frames_per_second)
	, Resource(resource_metafile)
{}
