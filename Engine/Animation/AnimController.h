#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "ResourceManagement/Resources/Animation.h"
#include "ResourceManagement/Resources/Skeleton.h"
#include <map>
#include <vector>
class AnimController
{
public:
	AnimController();
	~AnimController();
	void Init();

	void Play();
	void Stop();
	void Update(); 

	bool GetTranslation(const std::string& channel_name, float3& pos);
	bool GetRotation(const std::string& channel_name, Quat& rot);
	bool GetTransformation(const std::string& channel_name, float4x4 & transform);

	void GetChannelGlobalTransform(size_t channel_index, const Animation::Channel & channel, const std::vector<Animation::Channel> & channels, math::float4x4 &current_global_tranform);

public:
	std::shared_ptr<Animation> anim = nullptr;
	std::shared_ptr<Skeleton> sk = nullptr;
	std::map<size_t, std::vector<uint32_t>> channel_hierarchy_cache;

	bool loop = false;
	bool playing = false;

	int current_time = 0;
	int animation_time = 15000;
};

#endif //_ANIMCONTROLLER_H_

