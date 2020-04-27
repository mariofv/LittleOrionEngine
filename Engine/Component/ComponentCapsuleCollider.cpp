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

