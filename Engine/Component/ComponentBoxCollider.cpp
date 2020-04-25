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
