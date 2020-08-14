#include "ComponentSphereCollider.h"
#include "Main/Application.h"
#include "Main/GameObject.h"
#include "Module/ModulePhysics.h"

ComponentSphereCollider::ComponentSphereCollider() : ComponentCollider(ComponentCollider::ColliderType::SPHERE)
{
	InitData();
}

ComponentSphereCollider::ComponentSphereCollider(GameObject* owner) : ComponentCollider(owner, ComponentCollider::ColliderType::SPHERE)
{
	//box_size = btVector3(1.0F / 55.f, 1.0F / 55.f, 1.0F / 55.f);
	InitData();
}

Component* ComponentSphereCollider::Clone(GameObject* owner, bool original_prefab)
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
	created_component->SetConfiguration();
	CloneBase(static_cast<Component*>(created_component));

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);
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
	box_size = btVector3(scale.x, scale.y, scale.z);
}

void ComponentSphereCollider::InitData()
{
	col_shape = new btSphereShape(max(box_size.x(), box_size.y(), box_size.z()));
	AddBody();
}
