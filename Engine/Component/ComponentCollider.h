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
		SPHERE,
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

	virtual void UpdateDimensions() {}
	virtual void Scale() {}

	btRigidBody* AddBody();
	void MoveBody();
	void UpdateCommonDimensions();
	void SetMass(float new_mass);
	void SetVisualization();
	void SetCollisionDetection();

public:

	ColliderType collider_type;
	btRigidBody* body = nullptr;
	float mass = 1.0F; // 0.0F would create a static or inmutable body
	btDefaultMotionState* motion_state;
	float3 scale = float3(1.0F, 1.0F, 1.0F);
	float3 translation = float3::zero;
	Quat rotation = Quat::FromEulerXYZ(0.0F, 0.0F, 0.0F);
	btVector3 box_size = btVector3(1.0F, 1.0F, 1.0F);
	btCollisionShape* col_shape;
	btVector3 localInertia = btVector3(0.F, 0.F, 0.F);

	float3 deviation = float3::zero;
	bool visualize = true;
	bool detectCollision = true;
	bool is_attached = false;

};

#endif 
