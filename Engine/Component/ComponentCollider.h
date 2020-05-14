#ifndef _COMPONENTCOLLIDER_H
#define _COMPONENTCOLLIDER_H

#define ENGINE_EXPORTS

#include "Component.h"
#include "bullet3/btBulletDynamicsCommon.h"


class ComponentCollider : public Component
{
public:
	enum class ColliderType {
		BOX,
		CAPSULE,
		SPHERE,
		CYLINDER,
		MESH
	};

	ComponentCollider(ColliderType collider_type);
	ComponentCollider(GameObject* owner, ColliderType collider_type);
	~ComponentCollider() = default;

	void Copy(Component* component_to_copy) const override;
	void Delete() override;
	void Save(Config& config) const override;
	void Load(const Config &config) override;
	Component* Clone(bool original_prefab = false) const { return nullptr; }
	virtual Component* Clone(GameObject* owner, bool original_prefab = false) const { return nullptr; }

	void Enable();
	void Disable();

	virtual void UpdateDimensions() {}
	virtual void Scale() {}

	btRigidBody* AddBody();
	ENGINE_API void AddForce(float3& force);
	ENGINE_API void SetVelocity(float3& velocity, float speed);

	void MoveBody();
	void SetMass(float new_mass);
	void SetVisualization();
	void SetCollisionDetection();
	bool DetectCollision(); //returns true if collides with any object in the world
	bool DetectCollisionWith(ComponentCollider* collider); //returns true if collides with a concrete object
	void SetStatic();
	void SetRotationAxis();
	ENGINE_API void SwitchPhysics(bool active);
	void SwitchPhysics();
	void UpdateFriction();
	void SetRollingFriction();
	void SetConfiguration();
	void SetColliderCenter(float3& new_center);
	float3 GetColliderCenter() const;
	

protected:
	void CommonAssign(const ComponentCollider& component_to_copy);
	void UpdateCommonDimensions();

public:

	ColliderType collider_type;
	btRigidBody* body = nullptr;
	float mass = 1.0F; // 0.0F would create a static or inmutable body
	btDefaultMotionState* motion_state = nullptr;
	float3 scale = float3(1.0F, 1.0F, 1.0F);
	btVector3 box_size = btVector3(1.0F, 1.0F, 1.0F);
	btCollisionShape* col_shape;
	btVector3 local_inertia = btVector3(0.F, 0.F, 0.F);
	float friction = 1.0F;
	float rolling_friction = 1.0F;
	float3 center;
	float3 center_deviation = float3::zero;
	float3 deviation = float3::zero;
	bool visualize = true;
	bool detect_collision = true;
	bool is_attached = false;
	bool is_static = false;
	bool freeze_rotation_x = true;
	bool freeze_rotation_y = true;
	bool freeze_rotation_z = true;
	bool active_physics = true;

protected:
	friend class PanelComponent;

};

#endif 
