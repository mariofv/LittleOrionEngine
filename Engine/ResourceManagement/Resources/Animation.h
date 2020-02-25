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
	Animation(const char * const UID, const std::string & exported_file);
	Animation(std::vector<KeyFrame> && keyframes, std::string name, float duration,const std::string & exported_file);
	~Animation() = default;

private:
	void LoadInMemory() override {};

public:

	std::string name;
	std::vector<KeyFrame> keyframes;
	float duration;
};


namespace Loader
{
	template<>
	static std::shared_ptr<Animation> Load(const std::string& uid) {
		return AnimationLoader::Load(uid);
	}
}
