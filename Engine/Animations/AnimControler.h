#ifndef _ANIMCONTROLER_H_
#define _ANIMCONTROLER_H_

#include "ResourceManagement/Resources/Animation.h"

class AnimControler
{
public:
	AnimControler();
	~AnimControler();

public:
	bool loop;
	Animation* anim;
	unsigned int current_time = 0.f;
	unsigned int animation_time = anim->duration;

	float progress_animation = (current_time%animation_time / animation_time);


public:

	void Play(bool loop);
	void Stop();
	void Update(); //TIMER???

	void GetTransform(char* name, float3& pos, Quat& rot);
	
};

#endif _ANIMCONTROLER_H_

