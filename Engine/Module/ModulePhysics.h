#ifndef _MODULEPHYSICS_H_
#define _MODULEPHYSICS_H_

#include "Module.h"
#include "bullet3-2.89/src/btBulletDynamicsCommon.h"
#include "bullet3-2.89/src/LinearMath/btIDebugDraw.h"
#include "Helper/Timer.h"
#include "Math/float3.h"
#include "Main/GameObject.h"
#include "Component/ComponentBoxPrimitive.h"

#define BT_USE_FLOAT_PRECISION

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

	btRigidBody* AddBody(btVector3 box_size);
	void setGravity(float3 newGgravity);
	float3 getGravity();
	ComponentBoxPrimitive* CreateComponentBoxPrimitive(GameObject* owner);

public:

	btDefaultCollisionConfiguration* collision_conf = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broad_phase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;

	bool show_physics= false;
	Timer * physics_timer = nullptr;
	float ms = 0;
	std::vector<float> ms_info;
	std::vector<ComponentBoxPrimitive*> boxes;
	math::float3 gravity = float3(0.0f, -1.0f, 0.0f); //gravity world

private:

	int subSteps = 0;
	std::vector< btCollisionShape*> shapes;
	DebugDrawer* debug_draw;
	

};

#endif // !_MODULEPHYSICS_H_



