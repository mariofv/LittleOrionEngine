#ifndef _ANIMCONTROLER_H_
#define _ANIMCONTROLER_H_

#include "ResourceManagement/Resources/Animation.h"

class AnimControler
{
public:
	AnimControler();
	~AnimControler();

	void Play();
	void Stop();
	void Update(); 

	void GetTransform(char* name, float3& pos, Quat& rot);
	const float3 Interpolate(const float3 & first, const float3 & second, float lambda);
	const Quat Interpolate(const Quat & first, const Quat & second, float lambda);

public:
	bool loop = false;
	bool playing = false;

	Animation* anim = nullptr;

private:
	unsigned int current_time = 0;
	unsigned int animation_time = 600;
};

#endif _ANIMCONTROLER_H_

