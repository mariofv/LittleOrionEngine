#ifndef _ANIMCONTROLER_H_
#define _ANIMCONTROLER_H_

#include "ResourceManagement/Resources/Animation.h"

class AnimControler
{
public:
	AnimControler();
	~AnimControler();

public:
	bool loop = false;
	bool stop = false;
	Animation* anim = nullptr;
	unsigned int current_time = 0.f;
	unsigned int animation_time = 0.f;

	float progress_animation = (current_time%animation_time / animation_time);


public:

	void Play();
	void Stop();
	void Update(); //TIMER???

	void GetTransform(char* name, float3& pos, Quat& rot);

	const float3 Interpolate(const float3 & first, const float3 & second, float lambda);

	const Quat Interpolate(const Quat & first, const Quat & second, float lambda);
	
};

#endif _ANIMCONTROLER_H_

