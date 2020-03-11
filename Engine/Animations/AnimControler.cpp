#include "AnimControler.h"
#include "Main/Application.h"
#include "Module/ModuleResourceManager.h"



AnimControler::AnimControler()
{
	anim = App->resources->Load<Animation>("Library/Animations/2719853001_Take 001.anim").get();
}


AnimControler::~AnimControler()
{
}

void AnimControler::Play(bool loop)
{
	if (loop)
	{
		Update();
	}
		Play(loop);
}

void AnimControler::Stop()
{
	loop = false;
}

void AnimControler::Update()
{
	if (loop) {

	}
}

void AnimControler::GetTransform(char * name, float3 & pos, Quat & rot)
{
	//CurrentSample = (CurrentTime*(NumSamples - 1)) / AnimationDuration;
}
