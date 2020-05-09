#ifndef _COMPONENTMESHCOLLIDER_H
#define _COMPONENTMESHCOLLIDER_H

#include "ComponentCollider.h"


class ComponentMeshCollider : public ComponentCollider
{
public:

	ComponentMeshCollider();
	ComponentMeshCollider(GameObject* owner);
	~ComponentMeshCollider() = default;

	Component* Clone(GameObject* owner, bool original_prefab = false) const;
	ComponentMeshCollider& operator=(const ComponentMeshCollider& component_to_copy);

	void UpdateDimensions();
	void Scale();
private:
	void InitData();
	void CreateMeshBody();
private:
	std::vector<float> vertices;
	std::vector<int> indices;
};

#endif
