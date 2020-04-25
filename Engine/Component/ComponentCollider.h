#ifndef _COMPONENTCOLLIDER_H
#define _COMPONENTCOLLIDER_H

#include "Component.h"
#include "bullet3/btBulletDynamicsCommon.h"

class ComponentCollider : public Component
{
public:
	enum class ColliderType {
		BOX,
		CAPSULE,
		CIRCULE,
		MESH
	};

	ComponentCollider(ColliderType collider_type);
	ComponentCollider(GameObject* owner, ColliderType collider_type);
	~ComponentCollider() = default;

	void Copy(Component* component_to_copy) const override;
	void Delete() override;
	void Save(Config& config) const override;
	void Load(const Config &config) override;
	Component* Clone(bool original_prefab = false) const override;

	btRigidBody* AddBody();
	void MoveBody();
	void UpdateBoxDimensions();
	void SetMass(float new_mass);
	void SetVisualization();
	void SetCollisionDetection();
	void Scale();

	

public:

	ColliderType collider_type;
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

#endif 
