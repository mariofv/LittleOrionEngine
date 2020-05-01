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

Component* ComponentCylinderCollider::Clone(GameObject* owner, bool original_prefab) const
{
	ComponentCylinderCollider* created_component;
	if (original_prefab)
	{
		created_component = new ComponentCylinderCollider();
	}
	else
	{
		created_component = static_cast<ComponentCylinderCollider*> (App->physics->CreateComponentCollider(collider_type, owner));
	}
	*created_component = *this;
	return created_component;
}

ComponentCylinderCollider& ComponentCylinderCollider::operator=(const ComponentCylinderCollider& component_to_copy)
{
	col_shape = component_to_copy.col_shape;
	CommonAssign(component_to_copy);
	return *this;
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