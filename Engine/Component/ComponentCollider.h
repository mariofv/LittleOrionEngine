#ifndef _COMPONENTCOLLIDER_H
#define _COMPONENTCOLLIDER_H

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "Component.h"
#include <bullet3/btBulletDynamicsCommon.h>

class ComponentCollider;

struct CollisionInformation
{
	ComponentCollider* collider = nullptr;
	float distance = 0.0F;
	float3 normal = float3::zero;

};

struct ContactSensorCallback : public btCollisionWorld::ContactResultCallback
{
	struct CustomMainfoldPoint
	{
		int object_a_id;
		int object_b_id;
		btManifoldPoint point;
	};
	std::vector<CustomMainfoldPoint> mainfolds;

	virtual btScalar addSingleResult(
		btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0, int partId0, int index0,
		const btCollisionObjectWrapper* colObj1, int partId1, int index1
	)
	{
		CustomMainfoldPoint custom_mainfold{ colObj0->getCollisionObject()->getWorldArrayIndex(), colObj1->getCollisionObject()->getWorldArrayIndex(), cp };
		mainfolds.push_back(custom_mainfold);
		return 0;
	}
};


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

	void CopyTo(Component* component_to_copy) const override;
	void Delete() override;

	void SpecializedSave(Config& config) const override;
	void SpecializedLoad(const Config &config) override;

	virtual Component* Clone(GameObject* owner, bool original_prefab) override { return nullptr; }
	
	void Disable() override;
	void Enable() override;
	
	virtual void UpdateDimensions() {}
	virtual void Scale() {}

	btRigidBody* AddBody();

	void MoveBody();
	void SetMass(float new_mass);
	void SetVisualization();

	ENGINE_API void SetCollisionDetection();
	
	void SetStatic();
	void SetRotationAxis();

	ENGINE_API void SwitchPhysics(bool active);
	void SwitchPhysics();

	void UpdateFriction();

	void SetRollingFriction();
	void SetConfiguration();

	void SetColliderCenter(float3& new_center);
	float3 GetColliderCenter() const;

	ENGINE_API bool IsGrounded(float length_percentage = 0.75F);
	ENGINE_API std::vector<CollisionInformation> GetCollisions();

	ENGINE_API float3 GetOrigin() const;
	ENGINE_API float3 GetBoxSize() const;

	ENGINE_API CollisionInformation RaycastClosestHit(float3& start, float3& end) const;
	ENGINE_API std::vector<CollisionInformation> RaycastAllHits(float3& start, float3& end) const;

	ENGINE_API std::vector<CollisionInformation> CollisionTest() const; // This function returns true if the body of the collider is colliding, even thought outside step

	ENGINE_API std::vector<CollisionInformation> DetectAllCollision() const; //returns true if collides with any object in the world
	ENGINE_API bool IsCollidingWith(ComponentCollider* collider) const;
	ENGINE_API CollisionInformation DetectCollisionWith(ComponentCollider* collider) const; //returns true if collides with a concrete object
	ENGINE_API CollisionInformation DetectCollisionWithGround(float length_percentage = 0.75F) const;

protected:
	void CommonAssign(const ComponentCollider& component_to_copy);
	void UpdateCommonDimensions();

public:
	ColliderType collider_type;
	float mass = 1.0F; // 0.0F would create a static or inmutable body

	btDefaultMotionState* motion_state = nullptr;

	float3 scale = float3(1.0F, 1.0F, 1.0F);
	btVector3 box_size = btVector3(1.0F, 1.0F, 1.0F);

	btCollisionShape* col_shape;
	btVector3 local_inertia = btVector3(0.F, 0.F, 0.F);

	float friction = 1.0F;
	float rolling_friction = 1.0F;

	float3 center = float3::zero;

	bool visualize = true;

	bool detect_collision = true;
	bool is_attached = false;
	bool is_static = false;

	bool freeze_rotation_x = true;
	bool freeze_rotation_y = true;
	bool freeze_rotation_z = true;

	bool active_physics = true;

	float3 normal = float3::zero;

protected:
	btRigidBody* body = nullptr;

	friend class ModulePhysics;
	friend class PanelComponent;
};

#endif 
