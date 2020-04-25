#include "ComponentCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"



ComponentCollider::ComponentCollider(ColliderType collider_type) : Component(ComponentType::COLLIDER), collider_type(collider_type)
{

}

ComponentCollider::ComponentCollider(GameObject* owner, ColliderType collider_type) : Component(owner, ComponentType::COLLIDER), collider_type(collider_type)
{
	if (owner->aabb.global_bounding_box.IsFinite() && owner->aabb.global_bounding_box.Size().x != 0) {
		box_size = btVector3((owner->aabb.global_bounding_box.Size().x / 2) / owner->transform.GetGlobalScale().x,
			(owner->aabb.global_bounding_box.Size().y / 2) / owner->transform.GetGlobalScale().y,
			(owner->aabb.global_bounding_box.Size().z / 2) / owner->transform.GetGlobalScale().z);
		is_attached = true;
	}
}

Component* ComponentCollider::Clone(bool original_prefab) const
{
	return nullptr;
}

void ComponentCollider::Copy(Component* component_to_copy) const
{}

void ComponentCollider::Delete()
{
	App->physics->world->removeRigidBody(body);
	App->physics->RemoveComponentCollider(this);
}

void ComponentCollider::Save(Config & config) const
{
	config.AddUInt(UUID, "UUID");
	config.AddBool(active, "Active");
	config.AddUInt((unsigned int)type, "ComponentType");
	config.AddFloat(mass, "Mass");

}

void ComponentCollider::Load(const Config & config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	mass = config.GetFloat("Mass", 1.0F);
	AddBody();
}

btRigidBody* ComponentCollider::AddBody()
{
	if (body) {
		App->physics->world->removeRigidBody(body);
	}

	assert(col_shape);

	float3 global_scale = owner->transform.GetGlobalScale();
	col_shape->setLocalScaling(btVector3(global_scale.x * scale.x, global_scale.y * scale.y, global_scale.z * scale.z));
	float3 global_translation = owner->transform.GetGlobalTranslation();

	if (is_attached) {
		deviation = owner->aabb.global_bounding_box.CenterPoint() - global_translation;
		global_translation = owner->aabb.global_bounding_box.CenterPoint();
	}

	motion_state = new btDefaultMotionState(btTransform(btQuaternion(owner->transform.rotation.x, owner->transform.rotation.y, owner->transform.rotation.z, owner->transform.rotation.w), btVector3(global_translation.x, global_translation.y, global_translation.z)));

	if (mass != 0.f) col_shape->calculateLocalInertia(mass, localInertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state, col_shape, localInertia);
	body = new btRigidBody(rbInfo);

	App->physics->world->addRigidBody(body);

	return body;
}

void ComponentCollider::MoveBody()
{
	btTransform trans;
	motion_state->getWorldTransform(trans);
	owner->transform.SetGlobalMatrixTranslation(float3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ()) - deviation);
	owner->transform.SetRotation(Quat(trans.getRotation().x(), trans.getRotation().y(), trans.getRotation().z(), trans.getRotation().w()));
	if (is_attached)
	{
		deviation = owner->aabb.global_bounding_box.CenterPoint() - owner->transform.GetGlobalTranslation();
	}
}

void ComponentCollider::UpdateBoxDimensions()
{
	// This should only happen if the transform change
	float3 global_translation = owner->transform.GetGlobalTranslation();
	if (is_attached)
	{
		global_translation = owner->aabb.global_bounding_box.CenterPoint();
	}
	Quat global_rotation = owner->transform.GetGlobalRotation();
	float3 global_scale = owner->transform.GetGlobalScale();

	motion_state->setWorldTransform(btTransform(btQuaternion(global_rotation.x, global_rotation.y, global_rotation.z, global_rotation.w), btVector3(global_translation.x, global_translation.y, global_translation.z)));
	body->setMotionState(motion_state);
	body->getCollisionShape()->setLocalScaling(btVector3(global_scale.x * scale.x, global_scale.y * scale.y, global_scale.z * scale.z));
	if (is_attached)
	{
		deviation = owner->aabb.global_bounding_box.CenterPoint() - owner->transform.GetGlobalTranslation();
	}
}

void ComponentCollider::SetMass(float new_mass)
{

	if (mass == 0 && new_mass != 0) {
		body->getCollisionShape()->calculateLocalInertia(new_mass, body->getLocalInertia());
	}
	body->setMassProps(new_mass, localInertia);

}

void ComponentCollider::SetVisualization()
{
	if (!visualize) {
		body->setCollisionFlags(body->getCollisionFlags() | body->CF_DISABLE_VISUALIZE_OBJECT);
	}
	else {
		body->setCollisionFlags(~body->CF_DISABLE_VISUALIZE_OBJECT);
	}

}

void ComponentCollider::SetCollisionDetection()
{
	if (!detectCollision) {
		body->setCollisionFlags(body->getCollisionFlags() | body->CF_NO_CONTACT_RESPONSE);
	}
	else {
		body->setCollisionFlags(~body->CF_NO_CONTACT_RESPONSE);
	}
}

void ComponentCollider::Scale()
{
	body->getCollisionShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
}
