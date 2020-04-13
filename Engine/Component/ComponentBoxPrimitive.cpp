#include "ComponentBoxPrimitive.h"
#include "Main/Application.h"
#include "Module/ModulePhysics.h"





btRigidBody * ComponentBoxPrimitive::addBody(btVector3 box_size)
{
	mass = 1.0f; // 0.0f would create a static or inmutable body
	btCollisionShape* colShape = new btBoxShape(box_size); // regular box
	btMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(owner->transform.GetTranslation().x, owner->transform.GetTranslation().y, owner->transform.GetTranslation().z)));;
	btVector3 localInertia(0.f, 0.f, 0.f);
	if (mass != 0.f) colShape->calculateLocalInertia(mass, localInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, colShape, localInertia);
	body = new btRigidBody(rbInfo);
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




