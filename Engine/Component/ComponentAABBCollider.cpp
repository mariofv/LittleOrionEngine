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

ComponentAABBCollider::CollisionState ComponentAABBCollider::CheckAABBCollision(const AABB& reference_AABB) const
{
	static const size_t number_of_corners = 8;
	static const size_t number_of_planes = 6;

	//Get refence corners
	float3 reference_aabb_corners[number_of_corners];
	reference_AABB.GetCornerPoints(reference_aabb_corners);

	//Get own aabb planes
	Plane own_aabb_planes[number_of_planes];
	bounding_box.GetFacePlanes(own_aabb_planes);

	//Check if Corners are inside the planes
	int total_reference_planes_inside = 0;
	for (int p = 0; p < number_of_planes; ++p)
	{
		int points_inside_count = number_of_corners;
		int is_plane_inside = 1;
		for (int i = 0; i < number_of_corners; ++i)
		{
			if (!own_aabb_planes[p].IsOnPositiveSide(reference_aabb_corners[i])) //If true, the point is halfway or outside
			{
				// Plane is not inside
				is_plane_inside = 0; 
				--points_inside_count;
			}
		}
		if (points_inside_count == 0)
		{
			return CollisionState::OUTSIDE;
		}
		total_reference_planes_inside += is_plane_inside;
	}
	// so if total_reference_planes_inside is 6, then all are inside the view
	if (total_reference_planes_inside == 6)
	{
		return CollisionState::INTSIDE;
	}
	// we must be partly in then otherwise
	return CollisionState::INTERSECT;
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