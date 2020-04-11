#ifndef _MODULEPHYSICS_H_
#define _MODULEPHYSICS_H_

#include "Module.h"
#include "bullet3-2.89/src/btBulletDynamicsCommon.h"
#include "bullet3-2.89/src/LinearMath/btIDebugDraw.h"

class DebugDrawer : public btIDebugDraw
{
public:
	DebugDrawer() {}
	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB,
		btScalar distance, int lifeTime, const btVector3& color);
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

	btDefaultCollisionConfiguration* collision_conf = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broad_phase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;

private:

	int subSteps = 0;
	std::vector< btCollisionShape*> shapes;
	DebugDrawer* debug_draw;

};

#endif // !_MODULEPHYSICS_H_



