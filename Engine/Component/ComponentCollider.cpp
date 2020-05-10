#include "ComponentCollider.h"
#include "Component/ComponentMeshRenderer.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"

ComponentCollider::ComponentCollider(ColliderType collider_type) : Component(ComponentType::COLLIDER), collider_type(collider_type)
{

}

ComponentCollider::ComponentCollider(GameObject* owner, ColliderType collider_type) : Component(owner, ComponentType::COLLIDER), collider_type(collider_type)
{
	if (owner->aabb.global_bounding_box.IsFinite() && owner->aabb.global_bounding_box.Size().x != 0)
	{
		ComponentMeshRenderer* mesh = static_cast<ComponentMeshRenderer*>(owner->GetComponent(Component::ComponentType::MESH_RENDERER));
		if (mesh)
		{
			box_size = btVector3(owner->aabb.original_box.Size().x / 2,
				owner->aabb.original_box.Size().y / 2,
				owner->aabb.original_box.Size().z / 2);
		}
		else
		{
			box_size = btVector3(owner->aabb.global_bounding_box.Size().x / 2,
				owner->aabb.global_bounding_box.Size().y / 2,
				owner->aabb.global_bounding_box.Size().z / 2);
		}
		is_attached = true;
	}
}


void ComponentCollider::Copy(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentCollider*>(component_to_copy) = *this;
}


void ComponentCollider::CommonAssign(const ComponentCollider& component_to_copy)
{
	mass = component_to_copy.mass;
	scale = component_to_copy.scale;
	box_size = component_to_copy.box_size;
	visualize = component_to_copy.visualize;
	detect_collision = component_to_copy.detect_collision;
	is_attached = component_to_copy.is_attached;
	is_static = component_to_copy.is_static;
	AddBody();
}

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
	config.AddUInt((unsigned int)collider_type, "ColliderType");
	config.AddFloat(mass, "Mass");
	config.AddFloat3(scale, "Scale");
	config.AddBool(is_static, "Static");
	config.AddBool(detect_collision, "Collision");
	config.AddBool(visualize, "Visualize");
	config.AddBool(is_attached, "Attached");
	config.AddBool(x_axis, "x_axis");
	config.AddBool(y_axis, "y_axis");
	config.AddBool(z_axis, "z_axis");
	config.AddBool(disable_physics, "DisablePhysics");
	config.AddFloat(friction, "Friction");
	config.AddFloat(rolling_friction, "Rolling_friction");

}

void ComponentCollider::Load(const Config & config)
{
	UUID = config.GetUInt("UUID", 0);
	active = config.GetBool("Active", true);
	mass = config.GetFloat("Mass", 1.0F);
	config.GetFloat3("Scale", scale, float3::one);
	is_static = config.GetBool("Static", false);
	detect_collision = config.GetBool("Collision", true);
	visualize = config.GetBool("Visualize", true);
	is_attached = config.GetBool("Attached", false);
	x_axis = config.GetBool("x_axis", true);
	y_axis = config.GetBool("y_axis", true);
	z_axis = config.GetBool("z_axis", true);
	disable_physics = config.GetBool("DisablePhysics", false);
	friction = config.GetFloat("Friction", 1.0F);
	rolling_friction = config.GetFloat("Rolling_friction", 1.0F);
	AddBody();
	if (is_static) { SetStatic(); }
	if (!visualize) { SetVisualization(); }
	SetRotationAxis();
	if (!detect_collision) { SetCollisionDetection(); }
	if (disable_physics || !active) { DisablePhysics(); }
	UpdateFriction();
	SetRollingFriction();

}

btRigidBody* ComponentCollider::AddBody()
{
	if (body)
	{
		App->physics->world->removeRigidBody(body);
	}

	assert(col_shape);

	float3 global_scale = owner->transform.GetGlobalScale();
	col_shape->setLocalScaling(btVector3(global_scale.x * scale.x, global_scale.y * scale.y, global_scale.z * scale.z));

	float3 global_translation = owner->transform.GetGlobalTranslation();
	if (is_attached)
	{
		deviation = owner->aabb.global_bounding_box.CenterPoint() - global_translation;
		global_translation = owner->aabb.global_bounding_box.CenterPoint();
	}

	Quat global_rotation = owner->transform.GetGlobalRotation();
	motion_state = new btDefaultMotionState(btTransform(btQuaternion(global_rotation.x, global_rotation.y, global_rotation.z, global_rotation.w), btVector3(global_translation.x, global_translation.y, global_translation.z)));
	
	if (mass != 0.f) col_shape->calculateLocalInertia(mass, local_inertia);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state, col_shape, local_inertia);
	body = new btRigidBody(rbInfo);
	body->setActivationState(DISABLE_DEACTIVATION);
	body->setFriction(friction);
	App->physics->world->addRigidBody(body);
	
	if (collider_type == ComponentCollider::ColliderType::MESH) {
		SetVisualization();
	}
	
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

void ComponentCollider::UpdateCommonDimensions()
{
	float3 global_translation = owner->transform.GetGlobalTranslation();
	if (is_attached)
	{
		global_translation = owner->aabb.global_bounding_box.CenterPoint();
	}
	Quat global_rotation = owner->transform.GetGlobalRotation();
	motion_state->setWorldTransform(btTransform(btQuaternion(global_rotation.x , global_rotation.y , global_rotation.z , global_rotation.w ), btVector3(global_translation.x, global_translation.y, global_translation.z)));
	body->setMotionState(motion_state);

	if (is_attached)
	{
		deviation = owner->aabb.global_bounding_box.CenterPoint() - owner->transform.GetGlobalTranslation();
	}
	App->physics->world->updateSingleAabb(body);
}

void ComponentCollider::SetMass(float new_mass)
{
	App->physics->world->removeRigidBody(body);
	body->getCollisionShape()->calculateLocalInertia(new_mass, local_inertia);
	body->setMassProps(new_mass, local_inertia);
	App->physics->world->addRigidBody(body);
}

void ComponentCollider::SetVisualization()
{
	int flags = body->getCollisionFlags();
	if (!visualize)
	{
		flags |= body->CF_DISABLE_VISUALIZE_OBJECT;
		body->setCollisionFlags(flags);
	}
	else
	{
		flags -= body->CF_DISABLE_VISUALIZE_OBJECT;
		body->setCollisionFlags(flags);
	}

}

void ComponentCollider::SetCollisionDetection()
{
	int flags = body->getCollisionFlags();
	if (!detect_collision)
	{
		flags |= body->CF_NO_CONTACT_RESPONSE;
		body->setCollisionFlags(flags);
	}
	else
	{
		flags -= body->CF_NO_CONTACT_RESPONSE;
		body->setCollisionFlags(flags);
	}
}

bool ComponentCollider::DetectCollision()
{

	int numManifolds = App->physics->world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = App->physics->world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint pt = contactManifold->getContactPoint(j);
			if (obA->getWorldArrayIndex() == body->getWorldArrayIndex() || obB->getWorldArrayIndex() == body->getWorldArrayIndex())
			{
				if (pt.getDistance() < 0.0f)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool ComponentCollider::DetectCollisionWith(ComponentCollider * collider)
{

	int numManifolds = App->physics->world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = App->physics->world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint pt = contactManifold->getContactPoint(j);
			if ((obA->getWorldArrayIndex() == body->getWorldArrayIndex() && obB->getWorldArrayIndex() == collider->body->getWorldArrayIndex()) || (obB->getWorldArrayIndex() == body->getWorldArrayIndex() && obA->getWorldArrayIndex() == collider->body->getWorldArrayIndex()))
			{
				if (pt.getDistance() < 0.0f)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void ComponentCollider::SetStatic()
{
	int flags = body->getCollisionFlags();
	if (is_static)
	{
		flags |= body->CF_KINEMATIC_OBJECT;
		body->setCollisionFlags(flags);
		mass = 0.0F;
	}
	else
	{
		flags -= body->CF_KINEMATIC_OBJECT;
		body->setCollisionFlags(flags);
		mass = 1.0F;
	}
	
}

void ComponentCollider::SetRotationAxis()
{
	body->setAngularFactor(btVector3(int(x_axis), int(y_axis), int(z_axis)));
}

void ComponentCollider::AddForce(float3& force)
{
	body->applyCentralForce(btVector3(force.x, force.y, force.z));
}

void ComponentCollider::DisablePhysics(bool disable)
{
	disable_physics = disable;
	DisablePhysics();
}

void ComponentCollider::DisablePhysics()
{
	if (disable_physics || !active)
	{
		body->forceActivationState(DISABLE_SIMULATION);
	}
	else
	{
		body->forceActivationState(ACTIVE_TAG);
		/*body->forceActivationState(DISABLE_DEACTIVATION);*/
	}
}

void ComponentCollider::UpdateFriction()
{
	body->setFriction(friction);
}

void ComponentCollider::SetRollingFriction()
{
	body->setRollingFriction(rolling_friction);
}

void ComponentCollider::SetVelocity(float3& velocity, float speed)
{
	//bottom of the model
	btVector3 bottom = body->getWorldTransform().getOrigin();
	bottom.setY(bottom.getY() - 2*box_size.getY());

	//Vector normal to the surface
	btVector3 Normal; 
	App->physics->RaycastWorld(body->getWorldTransform().getOrigin(), bottom, Normal);
	float3 normal = float3(Normal);
	normal.Normalize();

	float2 normal_2D = float2(normal.x, normal.y);
	float2 vector_vel = normal_2D.Perp();
	
	if (abs(velocity.x) > 0 || abs(velocity.z) > 0) 
	{
		velocity.Normalize();
		body->setLinearVelocity(speed*btVector3(velocity.x, -SignOrZero(velocity.x)* SignOrZero(normal.x)*abs(vector_vel.y), velocity.z));
		
		//rotate collider

		/*float3 direction = float3(velocity.x, -SignOrZero(velocity.x)* SignOrZero(normal.x)*abs(vector_vel.y), velocity.z);*/
		float3 direction = float3(velocity.x, 0, velocity.z); 
		Quat new_rotation = owner->transform.GetRotation().LookAt(float3::unitZ, direction.Normalized(), float3::unitY, float3::unitY);
		
		btTransform trans = body->getWorldTransform();
		btQuaternion transrot = trans.getRotation();

		transrot = btQuaternion(new_rotation.x, new_rotation.y, new_rotation.z, new_rotation.w);
		trans.setRotation(transrot);
		body->setWorldTransform(trans);
	}
	
}
