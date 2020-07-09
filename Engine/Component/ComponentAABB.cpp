#include "ComponentAABB.h"
#include "ComponentMeshRenderer.h"
#include "ComponentParticleSystem.h"
#include "Main/GameObject.h"
#include <Brofiler/Brofiler.h>

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

void ComponentAABB::SpecializedSave(Config& config) const
{

}

void ComponentAABB::SpecializedLoad(const Config& config)
{

}

void ComponentAABB::GenerateBoundingBox()
{
	BROFILER_CATEGORY("GenerateBoundingBox", Profiler::Color::Lavender);
	bool has_mesh = false;
	ComponentMeshRenderer* owner_mesh_renderer = static_cast<ComponentMeshRenderer*>(owner->GetComponent(ComponentType::MESH_RENDERER));
	has_mesh = owner_mesh_renderer != nullptr && owner_mesh_renderer->mesh_to_render != nullptr;
	ComponentParticleSystem* owner_particle_system= static_cast<ComponentParticleSystem*>(owner->GetComponent(ComponentType::PARTICLE_SYSTEM));

	if (has_mesh)
	{
		GenerateBoundingBoxFromVertices(owner_mesh_renderer->mesh_to_render->vertices);
	}
	else if(owner_particle_system)
	{ 
		GenerateBoundingBoxFromParticleSystem(*owner_particle_system);
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

void ComponentAABB::GenerateBoundingBoxFromParticleSystem(const ComponentParticleSystem& particle_system)
{
	bounding_box.SetNegativeInfinity();
	original_box.SetNegativeInfinity();
	switch (particle_system.type_of_particle_system)
	{
	case ComponentParticleSystem::TypeOfParticleSystem::SPHERE:
	{
		float radius = particle_system.particles_life_time*particle_system.velocity_particles_start;
		Sphere sphere(float3(0.f,0.f,0.f), radius);
		bounding_box.Enclose(sphere);
		original_box.Enclose(sphere);
		break;
	}
	case ComponentParticleSystem::TypeOfParticleSystem::BOX:
	{
		float min_x = static_cast<float>(particle_system.min_range_random_x);
		float max_x = static_cast<float>(particle_system.max_range_random_x);
		float min_z = static_cast<float>(particle_system.min_range_random_z);
		float max_z = static_cast<float>(particle_system.max_range_random_z);
		float height = particle_system.particles_life_time * particle_system.velocity_particles_start * 100.0f;

		AABB box(float3(min_x, 0.0f, min_z) / 100, float3(max_x, height, max_z) / 100);
		bounding_box.Enclose(box);
		original_box.Enclose(box);
		break;
	}
	case ComponentParticleSystem::TypeOfParticleSystem::CONE:
		float max_radius = max(particle_system.outer_radius, particle_system.inner_radius);
		float height = particle_system.particles_life_time*particle_system.velocity_particles_start;
		float3 min_point = -float3(max_radius);
		min_point.y = 0.0f;
		float3 max_point =  float3(max_radius);
		max_point.y = height;

		AABB box(min_point, max_point);
		bounding_box.Enclose(box);
		original_box.Enclose(box);
		break;
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
	CloneBase(static_cast<Component*>(created_component));
	return created_component;
}

