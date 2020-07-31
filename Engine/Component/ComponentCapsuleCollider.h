#ifndef _COMPONENTCAPSULECOLLIDER_H
#define _COMPONENTCAPSULECOLLIDER_H

#include "ComponentCollider.h"


class ComponentCapsuleCollider : public ComponentCollider
{
public:

	ComponentCapsuleCollider();
	ComponentCapsuleCollider(GameObject* owner);
	~ComponentCapsuleCollider() = default;


	Component* Clone(GameObject* owner, bool original_prefab = false);
	ComponentCapsuleCollider& operator=(const ComponentCapsuleCollider& component_to_copy);

	void UpdateDimensions();
	void Scale();
	void InitData();
};

#endif
