#pragma once
#include "Resource.h"
#include <Component/ComponentTransform.h>
class Animation : public Resource
{
public:


	struct JointPosition
	{
		float frame;
		float4x4 position;

	};
	struct Channel
	{
		std::string name;
		std::vector<JointPosition> positions;


	};
	Animation(const char * const UID, const std::string & exported_file);
	~Animation() = default;

	void Save(Config& config) const override {};
	void Load(const Config& config) override {};

private:
	void LoadInMemory() override {};

public:

	std::string name;
	std::vector<Channel> channels;
	float duration;
};


namespace Loader
{
	template<>
	static std::shared_ptr<Animation> Load(const std::string& uid) {
		return nullptr;//AnimationLoader::Load(uid);
	}
}
