#ifndef _COMPONENTCAPSULECOLLIDER_H
#define _COMPONENTCAPSULECOLLIDER_H

#include "ComponentCollider.h"


class ComponentCapsuleCollider : public ComponentCollider
{
public:

	ComponentCapsuleCollider();
	ComponentCapsuleCollider(GameObject* owner);
	~ComponentCapsuleCollider() = default;

	void UpdateDimensions();
	void Scale();
};

#endif
