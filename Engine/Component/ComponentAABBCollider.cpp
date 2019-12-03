#include "ComponentAABBCollider.h"
#include "ComponentCamera.h"
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

void ComponentAABBCollider::Render(const ComponentCamera &camera, const GLuint shader_program) const
{
	App->renderer->bounding_box_renderer->Render(camera, bounding_box, shader_program);
}

void ComponentAABBCollider::GenerateBoundingBox()
{
	bool has_mesh = false;
	ComponentMesh * owner_mesh = static_cast<ComponentMesh*>(owner->GetComponent(ComponentType::MESH));
	has_mesh = owner_mesh != nullptr;
	
	if (has_mesh)
	{
		GenerateBoundingBoxFromVertices(owner_mesh->vertices);
	}
	else
	{
		bounding_box = AABB(float3::zero, float3::zero);
	}
	
	bounding_box.TransformAsAABB(owner->transform.GetGlobalModelMatrix());
}

void ComponentAABBCollider::GenerateBoundingBoxFromVertices(const std::vector<ComponentMesh::Vertex> & vertices)
{
	bounding_box.SetNegativeInfinity();
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		bounding_box.Enclose(vertices[i].position);
	}
}