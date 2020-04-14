#ifndef _COMPONENTBOXPRIMITIVE_H
#define _COMPONENTBOXPRIMITIVE_H

#include "Component.h"
#include "bullet3-2.89/src/btBulletDynamicsCommon.h"
#include "Main/GameObject.h"



class GameObject;

class ComponentBoxPrimitive : public Component
{
public:

	btRigidBody* addBody(btVector3 box_size);
	btRigidBody* body;
	btScalar mass;
	btDefaultMotionState* motionState;

	ComponentBoxPrimitive(ComponentType componentType);
	ComponentBoxPrimitive(GameObject* owner, ComponentType componentType);
	~ComponentBoxPrimitive() = default;

	Component* Clone(bool original_prefab = false) const override;
	void Copy(Component* component_to_copy) const override;

	void Delete() override;
	void Save(Config& config) const override;
	void Load(const Config &config) override;
	
};

#endif // !_COMPONENTBOXPRIMITIVE_H

