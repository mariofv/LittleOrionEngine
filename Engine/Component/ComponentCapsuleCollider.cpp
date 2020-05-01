#include "ComponentCapsuleCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"

ComponentCapsuleCollider::ComponentCapsuleCollider() : ComponentCollider(ComponentCollider::ColliderType::CAPSULE)
{
	col_shape = new btCapsuleShape(max(box_size.x(), box_size.z()), box_size.y());
	AddBody();
}

ComponentCapsuleCollider::ComponentCapsuleCollider(GameObject* owner) : ComponentCollider(owner, ComponentCollider::ColliderType::CAPSULE)
{
	col_shape = new btCapsuleShape(max(box_size.x(), box_size.z()), box_size.y());
	AddBody();
}

Component* ComponentCapsuleCollider::Clone(GameObject* owner, bool original_prefab) const
{
	ComponentCapsuleCollider* created_component;
	if (original_prefab)
	{
		created_component = new ComponentCapsuleCollider();
	}
	else
	{
		created_component = static_cast<ComponentCapsuleCollider*> (App->physics->CreateComponentCollider(collider_type, owner));
	}
	*created_component = *this;
	return created_component;
}

ComponentCapsuleCollider& ComponentCapsuleCollider::operator=(const ComponentCapsuleCollider& component_to_copy)
{
	col_shape = component_to_copy.col_shape;
	CommonAssign(component_to_copy);
	return *this;
}

void ComponentCapsuleCollider::UpdateDimensions()
{
	UpdateCommonDimensions();
	Scale();
}

void ComponentCapsuleCollider::Scale()
{
	float3 global_scale = owner->transform.GetGlobalScale();
	body->getCollisionShape()->setLocalScaling(btVector3(max(global_scale.x, global_scale.z) * scale.x, global_scale.y * scale.y, scale.z));
}

