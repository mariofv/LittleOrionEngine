#include "ComponentBoxCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"


ComponentBoxCollider::ComponentBoxCollider() : ComponentCollider(nullptr, ComponentBoxCollider::ColliderType::BOX)
{
	InitData();
}

ComponentBoxCollider::ComponentBoxCollider(GameObject* owner): ComponentCollider(owner, ComponentBoxCollider::ColliderType::BOX)
{
	InitData();
}

Component* ComponentBoxCollider::Clone(GameObject* owner, bool original_prefab)
{
	ComponentBoxCollider* created_component;
	if (original_prefab)
	{
		created_component = new ComponentBoxCollider(owner);
	}
	else
	{
		created_component = static_cast<ComponentBoxCollider*> (App->physics->CreateComponentCollider(collider_type, owner));
	}
	*created_component = *this;
	created_component->SetConfiguration();
	created_component->center = center;
	CloneBase(static_cast<Component*>(created_component));
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
	box_size = btVector3(scale.x, scale.y, scale.z);
}

void ComponentBoxCollider::InitData()
{
	col_shape = new btBoxShape(btVector3(box_size));
	AddBody();
}
