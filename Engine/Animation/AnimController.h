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

	bool GetTranslation(const std::string& channel_name, float3& pos);
	bool GetRotation(const std::string& channel_name, Quat& rot);

public:
	Animation* anim = nullptr;

	bool loop = false;
	bool playing = false;

	int current_time = 0;
	int animation_time = 15000;
};

#endif //_ANIMCONTROLLER_H_

