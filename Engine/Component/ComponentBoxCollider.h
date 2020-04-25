#ifndef _COMPONENTBOXCOLLIDER_H
#define _COMPONENTBOXCOLLIDER_H

#include "ComponentCollider.h"

#include "bullet3/btBulletDynamicsCommon.h"

class ComponentBoxCollider : public ComponentCollider
{
public:

	ComponentBoxCollider();
	ComponentBoxCollider(GameObject* owner);
	~ComponentBoxCollider() = default;
	
};

#endif // !_COMPONENTBOXCOLLIDER_H
