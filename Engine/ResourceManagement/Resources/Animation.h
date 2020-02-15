#pragma once
#include "Resource.h"
#include <Component/ComponentTransform.h>
class Animation : public Resource
{
public:

	struct Channel
	{
		std::string joint_name;
		std::vector<float4x4> positions;

	};
	Animation(const char * const UID, const std::string & exported_file);
	~Animation() = default;

	void Save(Config& config) const override {};
	void Load(const Config& config) override {};

private:
	void LoadInMemory() override {};

public:
	float duration;
	float tickts;
};


namespace Loader
{
	template<>
	static std::shared_ptr<Animation> Load(const std::string& uid) {
		return nullptr;//AnimationLoader::Load(uid);
	}
}
