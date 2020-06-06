#ifndef _COMPONENTCYLINDERCOLLIDER_H
#define _COMPONENTCYLINDERCOLLIDER_H

#include "ComponentCollider.h"


class ComponentCylinderCollider : public ComponentCollider
{
public:

	ComponentCylinderCollider();
	ComponentCylinderCollider(GameObject* owner);
	~ComponentCylinderCollider() = default;

	Component* Clone(GameObject* owner, bool original_prefab = false) const;
	ComponentCylinderCollider& operator=(const ComponentCylinderCollider& component_to_copy);

	void UpdateDimensions();
	void Scale();
};

#endif
