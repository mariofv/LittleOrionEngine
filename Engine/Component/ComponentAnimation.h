#ifndef _COMPONENTANIMATION_H_
#define _COMPONENTANIMATION_H_

#include "Component.h"

class ComponentAnimation :	public Component
{
public:
	ComponentAnimation();
	ComponentAnimation(GameObject * owner);
	~ComponentAnimation();

	/*
	Controler: AnimControler;
	Resource: UID;
	*/
public:
	void OnPlay();
	void OnStop();
	void OnUpdate();
};

#endif //_COMPONENTANIMATION_H_

