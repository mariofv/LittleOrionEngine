#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "ResourceManagement/Resources/Skeleton.h"

#include <map>
#include <vector>
class AnimController
{
public:
	AnimController() = default;
	~AnimController() = default;
	void Init();

	void Play();
	void Stop();
	void Update(); 

	bool GetTranslation(const std::string& channel_name, float3& position);
	bool GetRotation(const std::string& channel_name, Quat& rotation);

private:
	void UpdateChannelsGlobalTransformation();
public:
	std::shared_ptr<Animation> animation = nullptr;
	//std::shared_ptr<Skeleton> skeleton = nullptr;

	std::map<size_t, std::vector<uint32_t>> channel_hierarchy_cache;
	std::vector<float4x4> channel_global_transformation;

	bool loop = false;
	bool playing = false;

	int current_time = 0;
	int animation_time = 0;
};

#endif //_ANIMCONTROLLER_H_

