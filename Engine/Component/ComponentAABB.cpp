#include "ComponentAABB.h"
#include "ComponentMeshRenderer.h"
#include "Main/GameObject.h"
#include "Brofiler/Brofiler.h"

ComponentAABB::ComponentAABB() : Component(nullptr, ComponentType::AABB)
{

}

ComponentAABB::ComponentAABB(GameObject* owner) : Component(owner, ComponentType::AABB)
{

}

void ComponentAABB::Copy(Component* component_to_copy) const
{ 
	*static_cast<ComponentAABB*>(component_to_copy) = *this; 
};

void ComponentAABB::Save(Config& config) const
{

}

void ComponentAABB::Load(const Config& config)
{

}

void ComponentAABB::GenerateBoundingBox()
{
	BROFILER_CATEGORY("GenerateBoundingBox", Profiler::Color::Lavender);
	bool has_mesh = false;
	ComponentMeshRenderer* owner_mesh = static_cast<ComponentMeshRenderer*>(owner->GetComponent(ComponentType::MESH_RENDERER));
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
	GenerateGlobalBoundingBox();
	if (owner->parent != nullptr)
	{
		owner->parent->aabb.GenerateGlobalBoundingBox();
	}
}

void ComponentAABB::GenerateBoundingBoxFromVertices(const std::vector<Mesh::Vertex>& vertices)
{
	bounding_box.SetNegativeInfinity();
	original_box.SetNegativeInfinity();
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		bounding_box.Enclose(vertices[i].position);
		original_box.Enclose(vertices[i].position);
	}
}

void ComponentAABB::GenerateGlobalBoundingBox()
{
	global_bounding_box = bounding_box;
	for (auto& child : owner->children)
	{
		global_bounding_box.Enclose(child->aabb.global_bounding_box);
	}
}

bool ComponentAABB::IsEmpty() const
{
	return bounding_box.Size().Length() == 0;
}

Component* ComponentAABB::Clone(bool original_prefab) const
{
	ComponentAABB * created_component;
	created_component = new ComponentAABB();
	*created_component = *this;
	return created_component;
}

