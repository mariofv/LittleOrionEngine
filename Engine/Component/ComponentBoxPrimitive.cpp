#include "ComponentBoxPrimitive.h"
#include "Main/Application.h"
#include "Module/ModulePhysics.h"



btRigidBody * ComponentBoxPrimitive::addBody()
{
	
	mass = 1.0f; // 0.0f would create a static or inmutable body
	col_shape = new btBoxShape(btVector3(box_size)); // regular box
	
	//motion_state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(owner->transform.translation.x, owner->transform.translation.y, owner->transform.translation.z)));
	motion_state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(owner->aabb.bounding_box.CenterPoint().x, owner->aabb.bounding_box.CenterPoint().y, owner->aabb.bounding_box.CenterPoint().z)));
	
	//motion_state->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(owner->transform.translation.x, owner->transform.translation.y, owner->transform.translation.z)));
	deviation.y = owner->aabb.bounding_box.CenterPoint().y - owner->transform.translation.y;
	deviation.x = owner->aabb.bounding_box.CenterPoint().x - owner->transform.translation.x;
	deviation.z = owner->aabb.bounding_box.CenterPoint().z - owner->transform.translation.z;
	
	btVector3 localInertia(0.f, 0.f, 0.f);
	if (mass != 0.f) col_shape->calculateLocalInertia(mass, localInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state, col_shape, localInertia);
	body = new btRigidBody(rbInfo);
	
	App->physics->world->addRigidBody(body);
	body->applyCentralForce(btVector3(0, 1, 0));
	
	return body;
}

ComponentBoxPrimitive::ComponentBoxPrimitive(ComponentType componentType):Component(componentType)
{
	addBody();
}

ComponentBoxPrimitive::ComponentBoxPrimitive(GameObject * owner, ComponentType componentType): Component(owner, componentType)
{
	//size will be bounding_box size already computed

	box_size = btVector3(owner->aabb.bounding_box.Size().x/2, owner->aabb.bounding_box.Size().y/2, owner->aabb.bounding_box.Size().z/2);

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
	
	auto it = std::find(App->physics->boxes.begin(), App->physics->boxes.end(), this);
	if (it != App->physics->boxes.end())
	{
		App->physics->world->removeCollisionObject(this->body);
		App->physics->boxes.erase(it);
		//delete *it;
	}
	
}

void ComponentBoxPrimitive::Save(Config & config) const
{
}

void ComponentBoxPrimitive::Load(const Config & config)
{
}

void ComponentBoxPrimitive::makeBoxStatic() {

	if (is_static) 
	{
		body->setMassProps(0, body->getLocalInertia());
	}
	else {
		btVector3 localInertia(0.f, 0.f, 0.f);
		col_shape->calculateLocalInertia(1, localInertia);
		body->setMassProps(1, localInertia);
	}

}




