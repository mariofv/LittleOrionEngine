#include "ComponentBoxCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"


ComponentBoxCollider::ComponentBoxCollider() : ComponentCollider(nullptr, ComponentBoxCollider::ColliderType::BOX)
{
	col_shape = new btBoxShape(btVector3(box_size));
	AddBody();
}

ComponentBoxCollider::ComponentBoxCollider(GameObject* owner): ComponentCollider(owner, ComponentBoxCollider::ColliderType::BOX)
{
	col_shape = new btBoxShape(btVector3(box_size));
	AddBody();
}

Component* ComponentBoxCollider::Clone(GameObject* owner, bool original_prefab) const
{
	ComponentBoxCollider* created_component;
	if (original_prefab)
	{
		created_component = new ComponentBoxCollider();
	}
	else
	{
		created_component = static_cast<ComponentBoxCollider*> (App->physics->CreateComponentCollider(collider_type, owner));
	}
	*created_component = *this;
	return created_component;
}

ComponentBoxCollider& ComponentBoxCollider::operator=(const ComponentBoxCollider& component_to_copy)
{
	col_shape = component_to_copy.col_shape;
	CommonAssign(component_to_copy);
	return *this;
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
