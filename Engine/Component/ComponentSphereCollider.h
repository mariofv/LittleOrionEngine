#ifndef _COMPONENTSPHERECOLLIDER_H
#define _COMPONENTSPHERECOLLIDER_H

#include "ComponentCollider.h"
#include "bullet3/btBulletDynamicsCommon.h"


class ComponentSphereCollider : public ComponentCollider
{
public:

	ComponentSphereCollider();
	ComponentSphereCollider(GameObject* owner);
	~ComponentSphereCollider() = default;

	void UpdateDimensions();
	void Scale();
};

#endif
