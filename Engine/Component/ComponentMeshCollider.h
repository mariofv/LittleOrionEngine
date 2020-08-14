#ifndef _COMPONENTMESHCOLLIDER_H
#define _COMPONENTMESHCOLLIDER_H

#include "ComponentCollider.h"

class Mesh;

class ComponentMeshCollider : public ComponentCollider
{
public:

	ComponentMeshCollider();
	ComponentMeshCollider(GameObject* owner);
	~ComponentMeshCollider() = default;

	Component* Clone(GameObject* owner, bool original_prefab = false);
	ComponentMeshCollider& operator=(const ComponentMeshCollider& component_to_copy);

	void UpdateDimensions();
	void Scale();
	void InitMeshCollider();

private:
	void InitData();
	void CreateMeshBody();
private:
	std::vector<float> vertices;
	std::vector<int> indices;
	std::shared_ptr<Mesh> mesh;
};

#endif
