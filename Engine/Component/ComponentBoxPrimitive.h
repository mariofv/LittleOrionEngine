#ifndef _COMPONENTBOXPRIMITIVE_H
#define _COMPONENTBOXPRIMITIVE_H

#include "Component.h"
#include "Main/GameObject.h"

#include "bullet3/btBulletDynamicsCommon.h"


class GameObject;

class ComponentBoxPrimitive : public Component
{
public:

	ComponentBoxPrimitive(ComponentType componentType);
	ComponentBoxPrimitive(GameObject* owner, ComponentType componentType);
	~ComponentBoxPrimitive() = default;

	btRigidBody* AddBody();
	void MoveBody();
	void MoveNoAttachedBody();
	void UpdateBoxDimensions();
	void UpdateNoAttachedBoxDimensions();
	void setMass(float new_mass);
	void setVisualization();
	void setCollisionDetection();
	void Scale();

	void Copy(Component* component_to_copy) const override;
	void Delete() override;
	void Save(Config& config) const override;
	void Load(const Config &config) override;
	Component* Clone(bool original_prefab = false) const override;

public:

	btRigidBody* body = nullptr;
	float mass = 1.0f; // 0.0f would create a static or inmutable body
	btDefaultMotionState* motion_state;
	float3 scale = float3(1.0f, 1.0f, 1.0f);
	float3 translation = float3::zero;
	btVector3 box_size = btVector3(1.0f, 1.0f, 1.0f);
	btCollisionShape* col_shape;
	btVector3 localInertia = btVector3(0.f, 0.f, 0.f);

	float3 deviation = float3::zero;
	bool visualize = true;
	bool detectCollision = true;
	bool is_attached = false;
	
};

#endif // !_COMPONENTBOXPRIMITIVE_H

