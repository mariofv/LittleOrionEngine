#include "ComponentBoxPrimitive.h"
#include "Main/Application.h"
#include "Module/ModulePhysics.h"



btRigidBody * ComponentBoxPrimitive::addBody()
{
	
	mass = 1.0f; // 0.0f would create a static or inmutable body
	btCollisionShape* col_shape = new btBoxShape(btVector3(box_size)); // regular box
	motion_state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(owner->transform.translation.x, owner->transform.translation.y, owner->transform.translation.z)));;
	
	btVector3 localInertia(0.f, 0.f, 0.f);
	if (mass != 0.f) col_shape->calculateLocalInertia(mass, localInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state, col_shape, localInertia);
	body = new btRigidBody(rbInfo);
	
	App->physics->world->addRigidBody(body);
	
	return body;
}

ComponentBoxPrimitive::ComponentBoxPrimitive(ComponentType componentType):Component(componentType)
{
	addBody();
}

ComponentBoxPrimitive::ComponentBoxPrimitive(GameObject * owner, ComponentType componentType): Component(owner, componentType)
{
	addBody();
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




