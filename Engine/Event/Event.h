#ifndef _EVENT_H_
#define _EVENT_H_

#include "Main/GameObject.h"

class Event
{
protected:
	virtual ~Event() {};
};

struct CollisionEvent : public Event {
	CollisionEvent(GameObject* a, GameObject* b) : entityA{ a }, entityB{ b } {}
	GameObject* entityA;
	GameObject* entityB;
};
#endif
