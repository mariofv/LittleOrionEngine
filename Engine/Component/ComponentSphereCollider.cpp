#include "ComponentSphereCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"

ComponentSphereCollider::ComponentSphereCollider() : ComponentCollider(ComponentCollider::ColliderType::SPHERE)
{
	col_shape = new btSphereShape(max(box_size.x(), box_size.y(), box_size.z()));
	AddBody();
}

ComponentSphereCollider::ComponentSphereCollider(GameObject* owner) : ComponentCollider(owner, ComponentCollider::ColliderType::SPHERE)
{
	col_shape = new btSphereShape(max(box_size.x(), box_size.y(), box_size.z()));
	AddBody();
}

Component* ComponentSphereCollider::Clone(GameObject* owner, bool original_prefab) const
{
	ComponentSphereCollider* created_component;
	if (original_prefab)
	{
		created_component = new ComponentSphereCollider();
	}
	else
	{
		created_component = static_cast<ComponentSphereCollider*> (App->physics->CreateComponentCollider(collider_type, owner));
	}
	*created_component = *this;
	return created_component;
}

ComponentSphereCollider& ComponentSphereCollider::operator=(const ComponentSphereCollider& component_to_copy)
{
	col_shape = component_to_copy.col_shape;
	CommonAssign(component_to_copy);
	return *this;
}

void ComponentSphereCollider::UpdateDimensions()
{
	UpdateCommonDimensions();
	Scale();
}

void ComponentSphereCollider::Scale()
{
	float3 global_scale = owner->transform.GetGlobalScale();
	body->getCollisionShape()->setLocalScaling(btVector3(max(global_scale.x, global_scale.y, global_scale.z) * scale.x, max(global_scale.x, global_scale.y, global_scale.z) * scale.x, max(global_scale.x, global_scale.y, global_scale.z)* scale.x));
}