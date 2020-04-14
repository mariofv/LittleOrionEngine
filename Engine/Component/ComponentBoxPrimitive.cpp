#include "ComponentBoxPrimitive.h"
#include "Main/Application.h"
#include "Module/ModulePhysics.h"



btRigidBody * ComponentBoxPrimitive::addBody(btVector3 box_size)
{
	
	mass = 1.0f; // 0.0f would create a static or inmutable body
	btCollisionShape* colShape = new btBoxShape(btVector3(box_size)); // regular box
	motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(owner->transform.translation.x, owner->transform.translation.y, owner->transform.translation.z)));;
	
	btVector3 localInertia(0.f, 0.f, 0.f);
	if (mass != 0.f) colShape->calculateLocalInertia(mass, localInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
	body = new btRigidBody(rbInfo);
	
	
	//body->getMotionState()->setWorldTransform(btTransform(btQuaternion(owner->transform.GetRotation().x, owner->transform.GetRotation().y, owner->transform.GetRotation().z, owner->transform.GetRotation().w), btVector3(owner->transform.translation.x, owner->transform.translation.y, owner->transform.translation.z)));
	

	//body->setActivationState(DISABLE_DEACTIVATION);
	App->physics->world->addRigidBody(body);
	
	
	return body;
	
}

ComponentBoxPrimitive::ComponentBoxPrimitive(ComponentType componentType):Component(componentType)
{
	btVector3 aux(1, 1, 1);
	addBody(aux);
}

ComponentBoxPrimitive::ComponentBoxPrimitive(GameObject * owner, ComponentType componentType): Component(owner, componentType)
{
	btVector3 aux(1, 1, 1);
	addBody(aux);
}

Component * ComponentBoxPrimitive::Clone(bool original_prefab) const
{
	return nullptr;
}

void ComponentBoxPrimitive::Copy(Component * component_to_copy) const
{
}

void ComponentBoxPrimitive::Delete()
{
}

void ComponentBoxPrimitive::Save(Config & config) const
{
}

void ComponentBoxPrimitive::Load(const Config & config)
{
}




