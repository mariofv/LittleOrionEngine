#include "ComponentBoxPrimitive.h"
#include "Main/Application.h"
#include "Module/ModulePhysics.h"


btRigidBody* ComponentBoxPrimitive::AddBody()
{
	if (body) {
		App->physics->world->removeRigidBody(body);
	}
	
	btCollisionShape* col_shape = new btBoxShape(box_size); // regular box
	motion_state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(owner->transform.translation.x, owner->transform.translation.y, owner->transform.translation.z)));
	
	btVector3 localInertia(0.f, 0.f, 0.f);
	if (mass != 0.f) col_shape->calculateLocalInertia(mass, localInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state, col_shape, localInertia);
	body = new btRigidBody(rbInfo);
	
	App->physics->world->addRigidBody(body);
	
	return body;
}

ComponentBoxPrimitive::ComponentBoxPrimitive(ComponentType componentType):Component(componentType)
{
	AddBody();
}

ComponentBoxPrimitive::ComponentBoxPrimitive(GameObject* owner, ComponentType componentType): Component(owner, componentType)
{
	AddBody();
}

Component* ComponentBoxPrimitive::Clone(bool original_prefab) const
{
	return nullptr;
}

void ComponentBoxPrimitive::Copy(Component* component_to_copy) const
{
}

void ComponentBoxPrimitive::Delete()
{

	App->physics->world->removeRigidBody(body);
	App->physics->RemoveComponentBoxPrimitive(this);
}

void ComponentBoxPrimitive::Save(Config & config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddBool(active, "Active");
	config.AddUInt((unsigned int)type, "ComponentType");
	config.AddFloat(mass, "Mass");

}

void ComponentBoxPrimitive::Load(const Config & config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	mass = config.GetFloat("Mass", 1.0F);
	AddBody();
}
