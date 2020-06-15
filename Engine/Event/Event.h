#ifndef _EVENT_H_
#define _EVENT_H_

#include "Component/ComponentCollider.h"
#include "Main/GameObject.h"

class Event
{
public:
	GameObject* emitter = nullptr;
	Event(GameObject* emitter) : emitter(emitter) {}
	virtual ~Event() {};
};

struct CollisionEvent : public Event {
	CollisionEvent(GameObject* emitter, std::vector<CollisionInformation> collisions) : Event(emitter), collisions(collisions) {}
	std::vector<CollisionInformation> collisions;
};
#endif
