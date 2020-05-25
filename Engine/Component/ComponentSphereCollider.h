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

	Component* Clone(GameObject* owner, bool original_prefab = false) const;
	ComponentSphereCollider& operator=(const ComponentSphereCollider& component_to_copy);

	void UpdateDimensions();
	void Scale();
};

#endif
