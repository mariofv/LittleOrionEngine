#include "ModulePhysics.h"
#include "Main/Application.h"
#include "ModuleTime.h"
#include <GL/glew.h>

ModulePhysics::ModulePhysics()
{
	subSteps = 15;
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init()
{
	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	btVector3 aux(1000, 1000, 1000);
	AddBody(aux);



	return true;
}

update_status ModulePhysics::PreUpdate()
{
	//update the world
	world->stepSimulation(App->time->delta_time, subSteps);

	return update_status::UPDATE_CONTINUE;
}

update_status ModulePhysics::Update()
{
	world->debugDrawWorld();
	return update_status::UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	return true;
}


btRigidBody* ModulePhysics::AddBody(btVector3 box_size)
{
	
	btScalar mass = 1.0f; // 0.0f would create a static or inmutable body
	btCollisionShape* colShape = new btBoxShape(box_size); // regular box
	btMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));;
	shapes.push_back(colShape);
	btVector3 localInertia(0.f, 0.f, 0.f);
	if (mass != 0.f) colShape->calculateLocalInertia(mass, localInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);

	debug_draw = new DebugDrawer();
	debug_draw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	world->setDebugDrawer(debug_draw);
	
	
	return body;
}

void DebugDrawer::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
{
	/*mode = btIDebugDraw::DBG_DrawWireframe;
	if (mode> 0){
		float tmp[6] = { from.getX(), from.getY(), from.getZ(),
					  to.getX(), to.getY(), to.getZ() };

		
		glPushMatrix();
		{
			
			glColor4f(color.getX(), color.getY(), color.getZ(), 1.0f);
			glVertexPointer(3,
				GL_FLOAT,
				0,
				&tmp);

			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, 2);
			glDrawArrays(GL_LINES, 0, 2);
		}
		glPopMatrix();
	}
	*/
}

void DebugDrawer::drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color)
{
}

void DebugDrawer::reportErrorWarning(const char * warningString)
{
	printf(warningString);
}

void DebugDrawer::draw3dText(const btVector3 & location, const char * textString)
{
}

void DebugDrawer::setDebugMode(int debugMode)
{
	
}

int DebugDrawer::getDebugMode() const
{
	
	return btIDebugDraw::DBG_DrawWireframe;
}
