#include "ComponentAABBCollider.h"
#include "GameObject.h"
#include "ComponentMesh.h"

ComponentAABBCollider::ComponentAABBCollider() : Component(nullptr, ComponentType::MATERIAL)
{

}

ComponentAABBCollider::ComponentAABBCollider(GameObject * owner) : Component(owner, ComponentType::AABBCOLLIDER)
{

}

ComponentAABBCollider::~ComponentAABBCollider()
{

}

void ComponentAABBCollider::Enable()
{
	active = true;
}

void ComponentAABBCollider::Disable()
{
	active = false;
}

void ComponentAABBCollider::Update()
{

}

void ComponentAABBCollider::GenerateBoundingBox()
{

	for (unsigned int i = 0; i < owner->children.size(); ++i)
	{
		owner->children[i]->aabb_collider.GenerateBoundingBox();
	}

	bool has_meshes = false;
	bounding_box.SetNegativeInfinity();
	ComponentMesh * ownerMesh = static_cast<ComponentMesh*>(owner->GetComponent(ComponentType::MESH));
	has_meshes = ownerMesh != nullptr;
	if (has_meshes)
	{
		bounding_box.Enclose(ownerMesh->bounding_box);
	}
	else
	{
		bounding_box = AABB(float3::zero, float3::zero);
	}
	bounding_box.TransformAsAABB(owner->transform.GetGlobalModelMatrix());


	for (unsigned int i = 0; i < owner->children.size(); ++i)
	{
		bounding_box.Enclose(owner->children[i]->aabb_collider.bounding_box);
	}
}