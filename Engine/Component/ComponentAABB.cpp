#include "Application.h"
#include "ComponentAABB.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "Module/ModuleRender.h"

ComponentAABB::ComponentAABB() : Component(nullptr, ComponentType::MATERIAL)
{

}

ComponentAABB::ComponentAABB(GameObject * owner) : Component(owner, ComponentType::AABB)
{

}

void ComponentAABB::Save(Config& config) const
{

}

void ComponentAABB::Load(const Config& config)
{

}

void ComponentAABB::GenerateBoundingBox()
{
	bool has_mesh = false;
	ComponentMesh * owner_mesh = static_cast<ComponentMesh*>(owner->GetComponent(ComponentType::MESH));
	has_mesh = owner_mesh != nullptr;
	
	if (has_mesh)
	{
		GenerateBoundingBoxFromVertices(owner_mesh->mesh_to_render->vertices);
	}
	else
	{
		bounding_box = AABB(float3::zero, float3::zero);
	}
	
	bounding_box.TransformAsAABB(owner->transform.GetGlobalModelMatrix());

	float2 min_point2D = float2(bounding_box.MinX(), bounding_box.MinZ());
	float2 max_point2D = float2(bounding_box.MaxX(), bounding_box.MaxZ());
	bounding_box2D = AABB2D(min_point2D, max_point2D);
}

void ComponentAABB::GenerateBoundingBoxFromVertices(const std::vector<Mesh::Vertex> & vertices)
{
	bounding_box.SetNegativeInfinity();
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		bounding_box.Enclose(vertices[i].position);
	}
}

bool ComponentAABB::IsEmpty() const
{
	return bounding_box.Size().Length() == 0;
}

std::vector<float> ComponentAABB::GetVertices() const
{
	static const int num_of_vertices = 8;
	float3 tmp_vertices[num_of_vertices];
	bounding_box.GetCornerPoints(&tmp_vertices[0]);

	std::vector<float> vertices(num_of_vertices * 3);
	for (unsigned int i = 0; i < num_of_vertices; ++i)
	{
		vertices[i * 3] = tmp_vertices[i].x;
		vertices[i * 3 + 1] = tmp_vertices[i].y;
		vertices[i * 3 + 2] = tmp_vertices[i].z;
	}

	return vertices;
}

