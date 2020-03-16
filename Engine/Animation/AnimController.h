#ifndef _ANIMCONTROLLER_H_
#define _ANIMCONTROLLER_H_

#include "ResourceManagement/Resources/Animation.h"

class AnimController
{
public:
	AnimController();
	~AnimController();

	void Play();
	void Stop();
	void Update(); 

	bool GetTransform(const std::string& channel_name, float3& pos, Quat& rot);
	const float3 Interpolate(const float3 & first, const float3 & second, float lambda);
	const Quat Interpolate(const Quat & first, const Quat & second, float lambda);

public:
	Animation* anim = nullptr;

	bool loop = false;
	bool playing = false;

	int current_time = 0;
	int animation_time = 15000;
};

#endif //_ANIMCONTROLLER_H_

