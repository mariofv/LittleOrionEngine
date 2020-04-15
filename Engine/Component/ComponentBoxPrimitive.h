#ifndef _COMPONENTBOXPRIMITIVE_H
#define _COMPONENTBOXPRIMITIVE_H

#include "Component.h"
#include "Main/GameObject.h"

#include "bullet3-2.89/src/btBulletDynamicsCommon.h"


class GameObject;

class ComponentBoxPrimitive : public Component
{
public:

	ComponentBoxPrimitive(ComponentType componentType);
	ComponentBoxPrimitive(GameObject* owner, ComponentType componentType);
	~ComponentBoxPrimitive() = default;

	btRigidBody* AddBody();

	void Copy(Component* component_to_copy) const override;
	void Delete() override;
	void Save(Config& config) const override;
	void Load(const Config &config) override;
	Component* Clone(bool original_prefab = false) const override;

public:

	btRigidBody* body = nullptr;
	btScalar mass = 1.0f; // 0.0f would create a static or inmutable body
	btDefaultMotionState* motion_state;
	float3 scale = float3(1.0f, 1.0f, 1.0f);
	float3 translation = float3::zero;
	btVector3 box_size = btVector3(1.0f, 1.0f, 1.0f);
	//bool is_static = false;

	
};

#endif // !_COMPONENTBOXPRIMITIVE_H

