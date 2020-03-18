#pragma once
#include "Resource.h"
#include <Component/ComponentTransform.h>
#include <ResourceManagement/Loaders/AnimationLoader.h>
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
	Animation(const uint32_t UID, const std::string & exported_file);
	Animation(std::vector<KeyFrame> && keyframes, std::string name, float frames, float frames_per_second,const std::string & exported_file);
	~Animation() = default;

private:
	void LoadInMemory() override {};

public:

	std::string name;
	std::vector<KeyFrame> keyframes;
	float frames;
	float frames_per_second;
};


namespace Loader
{
	template<>
	static std::shared_ptr<Animation> Load(const std::string& uid) {
		return AnimationLoader::Load(uid);
	}
}
