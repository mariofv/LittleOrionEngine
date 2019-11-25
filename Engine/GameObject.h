#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Globals.h"
#include "Component.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

public:
	std::vector<Component*> components;

};

#endif //_GAMEOBJECT_H_