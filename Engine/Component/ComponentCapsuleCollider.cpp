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

