#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Resource.h"

#include "Component/ComponentTransform.h"
#include "ResourceManagement/Manager/AnimationManager.h"

class Animation : public Resource
{
public:
	struct Channel
	{

		std::string name;
		float4x4 position;

	};
	struct KeyFrame
	{
		float frame;
		std::vector<Channel> channels;
	};

	Animation() = default;
	Animation(Metafile* resource_metafile) : Resource(resource_metafile) {};
	Animation(Metafile* resource_metafile, std::vector<KeyFrame> && keyframes, std::string name, float frames, float frames_per_second);
	~Animation() = default;

public:
	std::string name;
	std::vector<KeyFrame> keyframes;
	float frames;
	float frames_per_second;
};

#endif // _ANIMATION_H_