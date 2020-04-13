#include "ModulePhysics.h"
#include "Main/Application.h"
#include "ModuleTime.h"
#include "ModuleDebugDraw.h"
#include <GL/glew.h>
#include "EditorUI/DebugDraw.h"



ModulePhysics::ModulePhysics()
{
	subSteps = 1;
	
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
	world->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
	
	btVector3 aux(1, 1, 1);
	AddBody(aux);



	return true;
}

update_status ModulePhysics::PreUpdate()
{
	
	return update_status::UPDATE_CONTINUE;
}

update_status ModulePhysics::Update()
{
	//physics_timer->Start();
	//update the world
	world->stepSimulation(App->time->delta_time, subSteps);
	if (showPhysics) {
		world->debugDrawWorld();
	}

	//physics_timer->Stop();
	//ms = physics_timer->Read();
	//ms_info.push_back(ms);
	//ImGui::PlotLines("Miliseconds", &ms_info[0], ms_info.size(), 0, nullptr, 0, 80);
	
	
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
	btMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)));;
	shapes.push_back(colShape);
	btVector3 localInertia(0.f, 0.f, 0.f);
	if (mass != 0.f) colShape->calculateLocalInertia(mass, localInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);

	btScalar mass2 = 0.0f; // 0.0f would create a static or inmutable body
	btCollisionShape* colShape2 = new btBoxShape(box_size); // regular box
	btMotionState* motionState2 = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(1.2f, 5, 0)));;
	shapes.push_back(colShape2);
	btVector3 localInertia2(0.f, 0.f, 0.f);
	if (mass2 != 0.f) colShape->calculateLocalInertia(mass2, localInertia2);
	btRigidBody::btRigidBodyConstructionInfo rbInfo2(mass2, motionState2, colShape2, localInertia2);
	btRigidBody* body2 = new btRigidBody(rbInfo2);
	world->addRigidBody(body2);

	debug_draw = new DebugDrawer();
	debug_draw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	world->setDebugDrawer(debug_draw);
	
	
	return body;
}

void ModulePhysics::setGravity(float3 newGravity)
{
	world->setGravity(btVector3(newGravity.x, newGravity.y, newGravity.z));
}

ComponentBoxPrimitive * ModulePhysics::CreateComponentBoxPrimitive(GameObject* owner)
{
	ComponentBoxPrimitive * createdBox = new ComponentBoxPrimitive(owner, Component::ComponentType::BOXPRIMITIVE);
	boxes.push_back(createdBox);
	return createdBox;
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
	}*/

	/*
	glBegin(GL_LINES);
	glVertex3d(from.getX(), from.getY(), from.getZ());
	glVertex3d(to.getX(), to.getY(), to.getZ());
	glEnd();*/

	
	dd::line(float3(from.getX(), from.getY(), from.getZ()), float3(to.getX(), to.getY(), to.getZ()), float3(1.0f, 0.0f, 0.0f));
	
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
	mode = btIDebugDraw::DebugDrawModes::DBG_DrawWireframe;
}

int DebugDrawer::getDebugMode() const
{
	
	return btIDebugDraw::DBG_DrawWireframe;
}
