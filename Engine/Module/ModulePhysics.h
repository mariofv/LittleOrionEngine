#ifndef _MODULEPHYSICS_H_
#define _MODULEPHYSICS_H_

#include "Module.h"
#include "bullet3/btBulletDynamicsCommon.h"
#include "bullet3/LinearMath/btIDebugDraw.h"
#include "Component/ComponentCollider.h"
#include "Helper/Timer.h"
#include "Math/float3.h"

#define BT_USE_FLOAT_PRECISION

class GameObject;

//Debug bullet draw
class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() {}
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int getDebugMode() const;
public:
	DebugDrawModes mode;
};


class ModulePhysics : public Module
{
public:
	
	ModulePhysics();
	~ModulePhysics();

	bool Init() override;
	bool CleanUp() override;
	update_status PreUpdate() override;
	update_status Update() override;

	void SetGravity(float3& newGgravity);
	float3 GetGravity();
	ComponentCollider* CreateComponentCollider(const ComponentCollider::ColliderType collider_type, GameObject* owner);
	void RemoveComponentCollider(ComponentCollider* collider_to_remove);
	bool RaycastWorld(const btVector3 &Start, btVector3 &End, btVector3 &Normal);

public:

	btDefaultCollisionConfiguration* collision_conf = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broad_phase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;

	bool show_physics = true;
	Timer* physics_timer = nullptr;
	float ms = 0;
	std::vector<float> ms_info;
	std::vector<ComponentCollider*> colliders;
	math::float3 gravity = float3(0.0f, -10.0f, 0.0f); //gravity world

private:

	int subSteps = 1;
	std::vector< btCollisionShape*> shapes;
	DebugDrawer* debug_draw;
	

};

#endif // !_MODULEPHYSICS_H_



