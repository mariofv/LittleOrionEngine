#ifndef _EVENT_H_
#define _EVENT_H_

#include "Component/ComponentCollider.h"
#include "Main/GameObject.h"

class Event
{
public:
	Event(GameObject* emitter) : emitter(emitter) {}
	virtual ~Event() {};

public:
	GameObject* emitter = nullptr;
};

class CollisionEvent : public Event {
public:
	CollisionEvent(GameObject* emitter, std::vector<CollisionInformation> collisions) : Event(emitter), collisions(collisions) {}
public:
	std::vector<CollisionInformation> collisions;
};
#endif
