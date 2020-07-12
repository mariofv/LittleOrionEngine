#include "ComponentCollider.h"
#include "Component/ComponentMeshRenderer.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Helper/Utils.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleDebugDraw.h"
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
			box_size = btVector3(
				owner->aabb.original_box.Size().x / 2,
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
	freeze_rotation_x = component_to_copy.freeze_rotation_x;
	freeze_rotation_y = component_to_copy.freeze_rotation_y;
	freeze_rotation_z = component_to_copy.freeze_rotation_z;
	friction = component_to_copy.friction;
	rolling_friction = component_to_copy.rolling_friction;
	visualize = component_to_copy.visualize;
	detect_collision = component_to_copy.detect_collision;
	is_attached = component_to_copy.is_attached;
	is_static = component_to_copy.is_static;
	active_physics = component_to_copy.active_physics;
	center = component_to_copy.center;
	AddBody();
}

void ComponentCollider::Delete()
{
	App->physics->world->removeRigidBody(body);
	App->physics->RemoveComponentCollider(this);
}

void ComponentCollider::SpecializedSave(Config & config) const
{
	config.AddUInt((unsigned int)collider_type, "ColliderType");
	config.AddFloat(mass, "Mass");
	config.AddFloat3(scale, "Scale");
	config.AddBool(is_static, "Static");
	config.AddBool(detect_collision, "Collision");
	config.AddBool(visualize, "Visualize");
	config.AddBool(is_attached, "Attached");
	config.AddBool(freeze_rotation_x, "Freeze_rotation_x");
	config.AddBool(freeze_rotation_y, "Freeze_rotation_y");
	config.AddBool(freeze_rotation_z, "Freeze_rotation_z");
	config.AddBool(active_physics, "Active_Physics");
	config.AddFloat(friction, "Friction");
	config.AddFloat(rolling_friction, "Rolling_friction");
	config.AddFloat3(center, "Center");
}

void ComponentCollider::SpecializedLoad(const Config & config)
{
	mass = config.GetFloat("Mass", 1.0F);
	config.GetFloat3("Scale", scale, float3::one);
	is_static = config.GetBool("Static", false);
	detect_collision = config.GetBool("Collision", true);
	visualize = config.GetBool("Visualize", true);
	is_attached = config.GetBool("Attached", false);
	freeze_rotation_x = config.GetBool("Freeze_rotation_x", true);
	freeze_rotation_y = config.GetBool("Freeze_rotation_y", true);
	freeze_rotation_z = config.GetBool("Freeze_rotation_z", true);
	active_physics = config.GetBool("Active_Physics", false);
	friction = config.GetFloat("Friction", 1.0F);
	rolling_friction = config.GetFloat("Rolling_friction", 1.0F);
	config.GetFloat3("Center", center, float3::zero);
	AddBody();
	SetConfiguration();

}

void ComponentCollider::Disable()
{
	active = false;
	SwitchPhysics();
	SetCollisionDetection();
}

void ComponentCollider::Enable()
{
	active = true;
	SwitchPhysics();
	SetCollisionDetection();
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
	Quat global_rotation = owner->transform.GetGlobalRotation();

	btTransform new_body_transformation = btTransform(
		btQuaternion(global_rotation.x, global_rotation.y, global_rotation.z, global_rotation.w),
		btVector3(global_translation.x, global_translation.y, global_translation.z)
	);

	btTransform center_of_mass = btTransform(
		btQuaternion::getIdentity(),
		btVector3(center.x, center.y, center.z)
	);

	motion_state = new btDefaultMotionState(new_body_transformation * center_of_mass);

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
	btTransform trans = body->getWorldTransform();

	float4x4 center_offset = float4x4::Translate(center);

	float4x4 new_global_matrix = float4x4::FromTRS(
		float3(
			trans.getOrigin().getX(),
			trans.getOrigin().getY(),
			trans.getOrigin().getZ()
		),
		Quat(
			trans.getRotation().x(),
			trans.getRotation().y(),
			trans.getRotation().z(),
			trans.getRotation().w()
		),
		float3::one
	) * center_offset.Inverted();

	owner->transform.SetGlobalModelMatrix(new_global_matrix);
}

void ComponentCollider::UpdateCommonDimensions()
{
	float3 global_translation = owner->transform.GetGlobalTranslation();
	Quat global_rotation = owner->transform.GetGlobalRotation();

	btTransform new_body_transformation = btTransform(
		btQuaternion(global_rotation.x, global_rotation.y, global_rotation.z, global_rotation.w), 
		btVector3(global_translation.x, global_translation.y, global_translation.z)
	);

	btTransform center_of_mass = btTransform(
		btQuaternion::getIdentity(),
		btVector3(center.x, center.y, center.z)
	);

	motion_state->setWorldTransform(new_body_transformation * center_of_mass);
	body->setMotionState(motion_state);
	
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
	flags |= body->CF_DISABLE_VISUALIZE_OBJECT;
	if (visualize)
	{
		flags -= body->CF_DISABLE_VISUALIZE_OBJECT;
		
	}
	body->setCollisionFlags(flags);
}

void ComponentCollider::SetCollisionDetection()
{
	int flags = body->getCollisionFlags();
	flags |= body->CF_NO_CONTACT_RESPONSE;

	if (detect_collision && active)
	{		
		flags &= ~(body->CF_NO_CONTACT_RESPONSE);
	}

	body->setCollisionFlags(flags);
}

std::vector<CollisionInformation> ComponentCollider::DetectAllCollision() const
{
	std::vector<CollisionInformation> collisions;
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
			if (pt.getDistance() < 0.0f
				&& (obA->getWorldArrayIndex() == body->getWorldArrayIndex()
					|| obB->getWorldArrayIndex() == body->getWorldArrayIndex()))
			{
				CollisionInformation info;
				info.distance = pt.getDistance();
				info.collider = App->physics->FindColliderByWorldId(obA->getWorldArrayIndex());
				info.normal = float3(-pt.m_normalWorldOnB);
				if (obB->getWorldArrayIndex() == body->getWorldArrayIndex())
				{
					info.collider = App->physics->FindColliderByWorldId(obB->getWorldArrayIndex());
					info.normal = float3(pt.m_normalWorldOnB);
				}
				collisions.push_back(info);
			}
		}
	}
	return collisions;
}
bool ComponentCollider::IsCollidingWith(ComponentCollider* collider) const {
	return DetectCollisionWith(collider).collider;
}

CollisionInformation ComponentCollider::DetectCollisionWith(ComponentCollider* collider) const
{
	
	CollisionInformation collision_info;

	if (detect_collision && collider->detect_collision && (active_physics || collider->active_physics))
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
				if (pt.getDistance() < 0.0f
					&& ((obA->getWorldArrayIndex() == body->getWorldArrayIndex()
						&& obB->getWorldArrayIndex() == collider->body->getWorldArrayIndex())
						|| (obB->getWorldArrayIndex() == body->getWorldArrayIndex()
							&& obA->getWorldArrayIndex() == collider->body->getWorldArrayIndex())))
				{
					collision_info.distance = pt.getDistance();
					collision_info.collider = App->physics->FindColliderByWorldId(obA->getWorldArrayIndex());
					collision_info.normal = float3(-pt.m_normalWorldOnB);
					if (obB->getWorldArrayIndex() == collider->body->getWorldArrayIndex())
					{
						collision_info.collider = App->physics->FindColliderByWorldId(obB->getWorldArrayIndex());
						collision_info.normal = float3(pt.m_normalWorldOnB);
					}
				}
			}
		}
	}
	else
	{
		btVector3 body_minim;
		btVector3 body_maxim;
		btVector3 collider_minim;
		btVector3 collider_maxim;

		App->physics->world->updateAabbs();
		body->getAabb(body_minim, body_maxim);
		collider->body->getAabb(collider_minim, collider_maxim);


		if (!(body_maxim.getX() < collider_minim.getX() || collider_maxim.getX() < body_minim.getX())
			&& !(body_maxim.getY() < collider_minim.getY() || collider_maxim.getY() < body_minim.getY())
			&& !(body_maxim.getZ() < collider_minim.getZ() || collider_maxim.getZ() < body_minim.getZ()))
		{
			collision_info.collider = collider;
		}
	}	
	return collision_info;
}

void ComponentCollider::SetStatic()
{
	int flags = body->getCollisionFlags();
	flags |= body->CF_KINEMATIC_OBJECT;
	float new_mass = 0.0F;
	if (!is_static)
	{
		flags -= body->CF_KINEMATIC_OBJECT;
		new_mass = mass;
	}
	mass = new_mass;
	body->setCollisionFlags(flags);
	
}

void ComponentCollider::SetRotationAxis()
{
	body->setAngularFactor(btVector3(int(!freeze_rotation_x), int(!freeze_rotation_y), int(!freeze_rotation_z)));
}

void ComponentCollider::SwitchPhysics(bool active)
{
	active_physics = active;
	SwitchPhysics();
}

void ComponentCollider::SwitchPhysics()
{
	if (active_physics && active)
	{
		body->forceActivationState(ACTIVE_TAG);
		body->setActivationState(DISABLE_DEACTIVATION);
	}
	else
	{
		body->forceActivationState(DISABLE_SIMULATION);
	}
}

std::vector<CollisionInformation> ComponentCollider::CollisionTest() const
{
	ContactSensorCallback contact_callback;
	App->physics->world->contactTest(body, contact_callback);
	std::vector<CollisionInformation> collisions;

	for (auto& custom_mainfold : contact_callback.mainfolds)
	{
		CollisionInformation collision;
		if (custom_mainfold.point.getDistance() < 0.0f)
		{
			if (custom_mainfold.object_a_id == body->getWorldArrayIndex())
			{
				collision.collider = App->physics->FindColliderByWorldId(custom_mainfold.object_b_id);
				collision.normal = float3(custom_mainfold.point.m_normalWorldOnB);
			}
			else if (custom_mainfold.object_b_id == body->getWorldArrayIndex())
			{
				collision.collider = App->physics->FindColliderByWorldId(custom_mainfold.object_a_id);
				collision.normal = -float3(custom_mainfold.point.m_normalWorldOnB);
			}
			else
			{
				assert(false); // This should never happen
			}
			collision.distance = -custom_mainfold.point.getDistance();

			collisions.push_back(collision);
		}
		
	}

	return collisions;
}

CollisionInformation ComponentCollider::RaycastClosestHit(float3& start, float3& end) const
{
	CollisionInformation info;
	btVector3 bullet_start = Utils::Float3TobtVector3(start);
	btVector3 bullet_end = Utils::Float3TobtVector3(end);

	btCollisionWorld::AllHitsRayResultCallback raycallback(bullet_start, bullet_end);

	App->physics->world->rayTest(bullet_start, bullet_end, raycallback);
	size_t num_hits = raycallback.m_hitPointWorld.size();
	for (size_t i = 0; i < num_hits; ++i)
	{
		end = float3(raycallback.m_hitPointWorld[i]);
		info.collider = App->physics->FindColliderByWorldId(raycallback.m_collisionObjects[i]->getWorldArrayIndex());
		info.normal = float3(raycallback.m_hitNormalWorld[i]);
		info.distance = (end - start).Length();
		break;
	}

	return info;
}

std::vector<CollisionInformation> ComponentCollider::RaycastAllHits(float3& start, float3& end) const
{
	std::vector<CollisionInformation> info;
	btVector3 bullet_start = Utils::Float3TobtVector3(start);
	btVector3 bullet_end = Utils::Float3TobtVector3(end);

	btCollisionWorld::AllHitsRayResultCallback raycallback(bullet_start, bullet_end);

	App->physics->world->rayTest(bullet_start, bullet_end, raycallback);
	size_t num_hits = raycallback.m_hitPointWorld.size();
	for (size_t i = 0; i < num_hits; ++i)
	{
		CollisionInformation collision;
		collision.collider = App->physics->FindColliderByWorldId(raycallback.m_collisionObjects[i]->getWorldArrayIndex());
		collision.normal = float3(raycallback.m_hitNormalWorld[i]);
		collision.distance = (end - start).Length();
	}

	return info;
}

void ComponentCollider::UpdateFriction()
{
	body->setFriction(friction);
}

void ComponentCollider::SetRollingFriction()
{
	body->setRollingFriction(rolling_friction);
}

void ComponentCollider::SetConfiguration()
{
	SetStatic(); 
	SetVisualization(); 
	SetRotationAxis();
	SetCollisionDetection(); 
	SwitchPhysics(); 
	UpdateFriction();
	SetRollingFriction();
}

void ComponentCollider::SetColliderCenter(float3& new_center)
{
	UpdateCommonDimensions();
}

float3 ComponentCollider::GetColliderCenter() const
{
	return center;
}

bool ComponentCollider::IsGrounded(float length_percentage)
{
	return DetectCollisionWithGround(length_percentage).collider;
}

CollisionInformation ComponentCollider::DetectCollisionWithGround(float length_percentage) const
{
	float3 origin = GetOrigin();
	float3 end = origin;
	end.y -= box_size.getY() * length_percentage;
	return RaycastClosestHit(origin, end);
}

std::vector<CollisionInformation> ComponentCollider::GetCollisions()
{
	std::vector<CollisionInformation> collisions;
	int numManifolds = App->physics->world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		CollisionInformation collision;

		btPersistentManifold* contactManifold = App->physics->world->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0();
		const btCollisionObject* obB = contactManifold->getBody1();

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.0f)
			{
				if (obA->getWorldArrayIndex() == body->getWorldArrayIndex() && obB->hasContactResponse())
				{
					collision.collider = App->physics->FindColliderByWorldId(obB->getWorldArrayIndex());
					collision.normal = float3(pt.m_normalWorldOnB.getX(), pt.m_normalWorldOnB.getY(), pt.m_normalWorldOnB.getZ());
					collision.distance = -pt.getDistance();
					collisions.emplace_back(collision);
				}
				else if (obB->getWorldArrayIndex() == body->getWorldArrayIndex() && obA->hasContactResponse())
				{
					collision.collider = App->physics->FindColliderByWorldId(obA->getWorldArrayIndex());
					collision.normal = -float3(pt.m_normalWorldOnB.getX(), pt.m_normalWorldOnB.getY(), pt.m_normalWorldOnB.getZ());
					collision.distance = -pt.getDistance();
					collisions.emplace_back(collision);
				}
			}
		}
	}
	return collisions;
}

float3 ComponentCollider::GetOrigin() const
{
	return float3(body->getWorldTransform().getOrigin());
}

float3 ComponentCollider::GetBoxSize() const
{
	return float3(box_size);
}