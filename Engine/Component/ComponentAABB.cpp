#include "ComponentAABB.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "Application.h"

#include "BoundingBoxRenderer.h"
#include "Module/ModuleRender.h"

ComponentAABB::ComponentAABB() : Component(nullptr, ComponentType::MATERIAL)
{

}

ComponentAABB::ComponentAABB(GameObject * owner) : Component(owner, ComponentType::AABB)
{

}

ComponentAABB::~ComponentAABB()
{

}

void ComponentAABB::Enable()
{
	active = true;
}

void ComponentAABB::Disable()
{
	active = false;
}

void ComponentAABB::Update()
{

}

void ComponentAABB::Render(const ComponentCamera &camera, const GLuint shader_program) const
{
	App->renderer->bounding_box_renderer->Render(camera, bounding_box, shader_program);
}

void ComponentAABB::GenerateBoundingBox()
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

void ComponentAABB::GenerateBoundingBoxFromVertices(const std::vector<ComponentMesh::Vertex> & vertices)
{
	bounding_box.SetNegativeInfinity();
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		bounding_box.Enclose(vertices[i].position);
	}
}