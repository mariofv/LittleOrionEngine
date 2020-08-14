#include "ComponentCylinderCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"

ComponentCylinderCollider::ComponentCylinderCollider() : ComponentCollider(ComponentCollider::ColliderType::CYLINDER)
{
	InitData();
}

ComponentCylinderCollider::ComponentCylinderCollider(GameObject* owner) : ComponentCollider(owner, ComponentCollider::ColliderType::CYLINDER)
{
	InitData();
}

Component* ComponentCylinderCollider::Clone(GameObject* owner, bool original_prefab) 
{
	ComponentCylinderCollider* created_component;
	if (original_prefab)
	{
		created_component = new ComponentCylinderCollider(owner);
	}
	else
	{
		created_component = static_cast<ComponentCylinderCollider*> (App->physics->CreateComponentCollider(collider_type, owner));
	}
	*created_component = *this;
	created_component->SetConfiguration();
	CloneBase(static_cast<Component*>(created_component));

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
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
	box_size = btVector3(scale.x, scale.y, scale.z);
}

void ComponentCylinderCollider::InitData()
{
	col_shape = new btCylinderShape(btVector3(box_size));
	AddBody();
}
