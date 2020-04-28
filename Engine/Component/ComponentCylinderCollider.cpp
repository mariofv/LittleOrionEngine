#include "ComponentCylinderCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"

ComponentCylinderCollider::ComponentCylinderCollider() : ComponentCollider(ComponentCollider::ColliderType::CYLINDER)
{
	col_shape = new btCylinderShape(btVector3(box_size));
	AddBody();
}

ComponentCylinderCollider::ComponentCylinderCollider(GameObject* owner) : ComponentCollider(owner, ComponentCollider::ColliderType::CYLINDER)
{
	col_shape = new btCylinderShape(btVector3(box_size));
	AddBody();
}

void ComponentCylinderCollider::UpdateDimensions()
{
	UpdateCommonDimensions();
	Scale();
}

void ComponentCylinderCollider::Scale()
{
	float3 global_scale = owner->transform.GetGlobalScale();
	body->getCollisionShape()->setLocalScaling(btVector3(max(global_scale.x, global_scale.z) * scale.x, global_scale.y * scale.y, scale.z));
}