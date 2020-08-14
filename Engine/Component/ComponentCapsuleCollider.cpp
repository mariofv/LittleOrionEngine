#include "ComponentCapsuleCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"

ComponentCapsuleCollider::ComponentCapsuleCollider() : ComponentCollider(ComponentCollider::ColliderType::CAPSULE)
{
	InitData();
}

ComponentCapsuleCollider::ComponentCapsuleCollider(GameObject* owner) : ComponentCollider(owner, ComponentCollider::ColliderType::CAPSULE)
{
	InitData();
}

Component* ComponentCapsuleCollider::Clone(GameObject* owner, bool original_prefab) 
{
	ComponentCapsuleCollider* created_component;
	if (original_prefab)
	{
		created_component = new ComponentCapsuleCollider(owner);
	}
	else
	{
		created_component = static_cast<ComponentCapsuleCollider*> (App->physics->CreateComponentCollider(collider_type, owner));
	}
	*created_component = *this;
	created_component->SetConfiguration();
	created_component->center = center;
	CloneBase(static_cast<Component*>(created_component));

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
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
	box_size = btVector3(scale.x, scale.y, scale.z);
}

void ComponentCapsuleCollider::InitData()
{
	col_shape = new btCapsuleShape(max(box_size.x(), box_size.z()), box_size.y());
	AddBody();
}

