#include "ComponentAABBCollider.h"
#include "GameObject.h"
#include "Application.h"

#include "BoundingBoxRenderer.h"
#include "Module/ModuleRender.h"

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

void ComponentAABBCollider::Render(const GLuint shader_program) const
{
	App->renderer->bounding_box_renderer->Render(bounding_box, shader_program);
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
		GenerateBoundingBoxFromVertices(ownerMesh->vertices);
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

void ComponentAABBCollider::GenerateBoundingBoxFromVertices(const std::vector<ComponentMesh::Vertex> & vertices)
{
	bounding_box.SetNegativeInfinity();
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		bounding_box.Enclose(vertices[i].position);
	}
}

void ComponentAABBCollider::GenerateBoundingBoxFromFrustum(const Frustum & frustum) 
{
	bounding_box.SetNegativeInfinity();
	for (unsigned int i = 0; i < 8 ; ++i)
	{
		bounding_box.Enclose(frustum.CornerPoint(i));
	}
		
}