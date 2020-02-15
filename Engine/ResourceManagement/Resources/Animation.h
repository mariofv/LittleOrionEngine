#pragma once
#include "Resource.h"

class Animation : public Resource
{
public:
	Animation() = default;
	~Animation() = default;

	void Save(Config& config) const override {};
	void Load(const Config& config) override {};

private:
	void LoadInMemory() override {};
};


namespace Loader
{
	template<>
	static std::shared_ptr<Animation> Load(const std::string& uid) {
		return nullptr;//AnimationLoader::Load(uid);
	}
}
