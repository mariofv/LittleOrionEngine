#include "ComponentBoxCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"


ComponentBoxCollider::ComponentBoxCollider() : ComponentCollider(nullptr, ComponentCollider::ColliderType::BOX)
{
	col_shape = new btBoxShape(btVector3(box_size));
	AddBody();
}

ComponentBoxCollider::ComponentBoxCollider(GameObject* owner): ComponentCollider(owner, ComponentCollider::ColliderType::BOX)
{
	col_shape = new btBoxShape(btVector3(box_size));
	AddBody();
}

void ComponentBoxCollider::UpdateDimensions()
{
	Scale();
	UpdateCommonDimensions();
}

void ComponentBoxCollider::Scale()
{
	float3 global_scale = owner->transform.GetGlobalScale();
	body->getCollisionShape()->setLocalScaling(btVector3(global_scale.x * scale.x, global_scale.y * scale.y, global_scale.z * scale.z));
}
