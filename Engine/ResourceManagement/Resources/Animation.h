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
		float3 translation;
		Quat rotation;
	};

	struct KeyFrame
	{
		float frame;
		std::vector<Channel> channels;
	};

	Animation() = default;
	Animation(uint32_t uuid) : Resource(uuid) {};
	Animation(uint32_t uuid, std::vector<KeyFrame> && keyframes, std::string name, float frames, float frames_per_second);
	~Animation() = default;

public:
	std::string name;
	std::vector<KeyFrame> keyframes;
	float frames;
	float frames_per_second;
};


namespace ResourceManagement
{
	template<>
	static std::shared_ptr<Animation> Load(uint32_t uuid, const FileData& resource_data, bool async)
	{
		return AnimationManager::Load(uuid, resource_data);
	}
}

#endif // _ANIMATION_H_