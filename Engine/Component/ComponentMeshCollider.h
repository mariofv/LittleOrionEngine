#ifndef _COMPONENTMESHCOLLIDER_H
#define _COMPONENTMESHCOLLIDER_H

#include "ComponentCollider.h"


class ComponentMeshCollider : public ComponentCollider
{
public:

	ComponentMeshCollider();
	ComponentMeshCollider(GameObject* owner);
	~ComponentMeshCollider() = default;

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
