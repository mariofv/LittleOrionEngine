#include "ModuleSpacePartitioning.h"

#include "Main/Application.h"
#include "Module/ModuleDebug.h"
#include "Module/ModuleScene.h"
#include "Module/ModuleRender.h"

#include "Component/Component.h"
#include "Component/ComponentMeshRenderer.h"

#include <algorithm>
#include <assimp/scene.h>
#include <MathGeoLib.h>
#include <Brofiler/Brofiler.h>

bool ModuleSpacePartitioning::Init()
{
	ol_abbtree = std::make_unique<OLAABBTree>(INITIAL_SIZE_AABBTREE);
	ol_quadtree = std::make_unique<OLQuadTree>();
	ol_octtree = std::make_unique<OLOctTree>();

	return true;
}

void ModuleSpacePartitioning::GenerateQuadTree()
{

	BROFILER_CATEGORY("Generate QuadTree", Profiler::Color::Lavender);
	AABB2D global_AABB;
	global_AABB.SetNegativeInfinity();

	for (const auto& mesh_renderer : App->renderer->mesh_renderers)
	{
		float minX = std::fmin(mesh_renderer->owner->aabb.bounding_box2D.minPoint.x, global_AABB.minPoint.x);
		float minY = std::fmin(mesh_renderer->owner->aabb.bounding_box2D.minPoint.y, global_AABB.minPoint.y);

		float maxX = std::fmax(mesh_renderer->owner->aabb.bounding_box2D.maxPoint.x, global_AABB.maxPoint.x);
		float maxY = std::fmax(mesh_renderer->owner->aabb.bounding_box2D.maxPoint.y, global_AABB.maxPoint.y);
		global_AABB.maxPoint = float2(maxX, maxY);
		global_AABB.minPoint = float2(minX, minY);
	}

	ol_quadtree->Create(global_AABB);
	for (const auto& mesh_renderer : App->renderer->mesh_renderers)
	{
		ol_quadtree->Insert(*mesh_renderer->owner);
	}
}

void ModuleSpacePartitioning::GenerateOctTree()
{

	BROFILER_CATEGORY("Generate OctTree", Profiler::Color::Lavender);
	AABB global_AABB;
	global_AABB.SetNegativeInfinity();

	for (const auto& mesh_renderer : App->renderer->mesh_renderers)
	{
		float minX = std::fmin(mesh_renderer->owner->aabb.bounding_box.minPoint.x, global_AABB.minPoint.x);
		float minY = std::fmin(mesh_renderer->owner->aabb.bounding_box.minPoint.y, global_AABB.minPoint.y);
		float minZ = std::fmin(mesh_renderer->owner->aabb.bounding_box.minPoint.z, global_AABB.minPoint.z);

		float maxX = std::fmax(mesh_renderer->owner->aabb.bounding_box.maxPoint.x, global_AABB.maxPoint.x);
		float maxY = std::fmax(mesh_renderer->owner->aabb.bounding_box.maxPoint.y, global_AABB.maxPoint.y);
		float maxZ = std::fmax(mesh_renderer->owner->aabb.bounding_box.maxPoint.z, global_AABB.maxPoint.z);

		global_AABB.maxPoint = float3(maxX, maxY, maxZ);
		global_AABB.minPoint = float3(minX, minY, minZ);
	}

	ol_octtree->Create(global_AABB);
	for (const auto&  mesh_renderer : App->renderer->mesh_renderers)
	{
		ol_octtree->Insert(*mesh_renderer->owner);
	}
}

void ModuleSpacePartitioning::InsertAABBTree(GameObject* game_object)
{
	ComponentMeshRenderer* object_mesh = static_cast<ComponentMeshRenderer*>(game_object->GetComponent(Component::ComponentType::MESH_RENDERER));
	if (object_mesh != nullptr)
	{
		//ol_abbtree->Insert(game_object);
	}
}

void ModuleSpacePartitioning::RemoveAABBTree(GameObject* game_object)
{
	ComponentMeshRenderer* object_mesh = static_cast<ComponentMeshRenderer*>(game_object->GetComponent(Component::ComponentType::MESH_RENDERER));
	if (object_mesh != nullptr)
	{
		//ol_abbtree->Remove(game_object);
	}
}

void ModuleSpacePartitioning::UpdateAABBTree(GameObject * game_object)
{
	BROFILER_CATEGORY("Update AABBTree", Profiler::Color::Lavender);
	if(game_object->IsStatic())
	{
		return;
	}

	ComponentMeshRenderer* object_mesh = static_cast<ComponentMeshRenderer*>(game_object->GetComponent(Component::ComponentType::MESH_RENDERER));
	if (object_mesh != nullptr)
	{
		//ol_abbtree->UpdateObject(game_object);
	}
}

void ModuleSpacePartitioning::ResetAABBTree()
{
	ol_abbtree.reset(new OLAABBTree(INITIAL_SIZE_AABBTREE));
}

void ModuleSpacePartitioning::DrawAABBTree() const
{
	BROFILER_CATEGORY("Render AABBTree", Profiler::Color::Lavender);

	ol_abbtree->Draw();
}

std::vector<ComponentMeshRenderer*> ModuleSpacePartitioning::GetCullingMeshes(
	const ComponentCamera* camera, 
	const std::vector<ComponentMeshRenderer*>& mesh_renderers, 
	int culling_filters
) const
{
	return GetCullingMeshes(camera->camera_frustum, mesh_renderers, culling_filters);
}

std::vector<ComponentMeshRenderer*> ModuleSpacePartitioning::GetCullingMeshes(const Frustum& camera_frustum, const std::vector<ComponentMeshRenderer*>& mesh_renderers, int culling_filters) const
{
	std::vector<ComponentMeshRenderer*> culled_mesh_renderers;

	switch (culling_mode)
	{
	case CullingMode::NONE:
		std::copy_if(
			mesh_renderers.begin(),
			mesh_renderers.end(),
			std::back_inserter(culled_mesh_renderers),
			[culling_filters](const auto& mesh_renderer)
		{
			return mesh_renderer->IsEnabled() && (culling_filters == 0 || mesh_renderer->CheckFilters(culling_filters));
		}
		);
		break;

	case CullingMode::FRUSTUM_CULLING:
		std::copy_if(
			mesh_renderers.begin(),
			mesh_renderers.end(),
			std::back_inserter(culled_mesh_renderers),
			[camera_frustum, culling_filters](const auto& mesh_renderer)
			{
				return 
					mesh_renderer->IsEnabled()
					&& mesh_renderer->owner->IsVisible(camera_frustum)
					&& (culling_filters == 0 || mesh_renderer->CheckFilters(culling_filters));
			}
		);
		break;

	case CullingMode::QUADTREE_CULLING:
	{
		// First we get all non static objects inside frustum
		std::copy_if(
			mesh_renderers.begin(),
			mesh_renderers.end(),
			std::back_inserter(culled_mesh_renderers),
			[camera_frustum, culling_filters](const auto& mesh_renderer)
		{

			return
				mesh_renderer->IsEnabled()
				&& mesh_renderer->owner->IsVisible(camera_frustum)
				&& !mesh_renderer->owner->IsStatic()
				&& (culling_filters == 0 || mesh_renderer->CheckFilters(culling_filters));
		}
		);

		// Then we add all static objects culled using the quadtree
		std::vector<GameObject*> rendered_objects;
		ol_quadtree->CollectIntersect(rendered_objects, camera_frustum);

		for (const auto& object : rendered_objects)
		{
			ComponentMeshRenderer* object_mesh = static_cast<ComponentMeshRenderer*>(object->GetComponent(Component::ComponentType::MESH_RENDERER));
			culled_mesh_renderers.push_back(object_mesh);
		}
		break;
	}
	case CullingMode::OCTTREE_CULLING:
	{
		// First we get all non static objects inside frustum
		std::copy_if(
			mesh_renderers.begin(),
			mesh_renderers.end(),
			std::back_inserter(culled_mesh_renderers),
			[camera_frustum, culling_filters](const auto& mesh_renderer)
			{
				return
					mesh_renderer->IsEnabled()
					&& mesh_renderer->owner->IsVisible(camera_frustum)
					&& !mesh_renderer->owner->IsStatic()
					&& (culling_filters == 0 || mesh_renderer->CheckFilters(culling_filters));
			}
		);

		// Then we add all static objects culled using the octtree
		std::vector<GameObject*> rendered_objects;
		ol_octtree->CollectIntersect(rendered_objects, camera_frustum);

		for (const auto& object : rendered_objects)
		{
			ComponentMeshRenderer *object_mesh = static_cast<ComponentMeshRenderer*>(object->GetComponent(Component::ComponentType::MESH_RENDERER));
			culled_mesh_renderers.push_back(object_mesh);
		}
		break;
	}

	case CullingMode::AABBTREE_CULLING:
	{
		// First we get all non static objects inside frustum
		std::copy_if(
			mesh_renderers.begin(),
			mesh_renderers.end(),
			std::back_inserter(culled_mesh_renderers),
			[camera_frustum, culling_filters](const auto& mesh_renderer)
			{

			return
				mesh_renderer->IsEnabled()
				&& mesh_renderer->owner->IsVisible(camera_frustum)
				&& mesh_renderer->owner->IsStatic()
				&& (culling_filters == 0 || mesh_renderer->CheckFilters(culling_filters));
			}
		);

		// Then we add all dynamic objects culled using the aabbtree
		std::vector<GameObject*> rendered_objects;
		ol_abbtree->GetIntersection(rendered_objects, camera_frustum);

		for (const auto& object : rendered_objects)
		{
			ComponentMeshRenderer *object_mesh = static_cast<ComponentMeshRenderer*>(object->GetComponent(Component::ComponentType::MESH_RENDERER));
			culled_mesh_renderers.push_back(object_mesh);
		}

		break;
	}
	case CullingMode::COMBINED_CULLING:
	{
		// We add all static objects culled using the quadtree
		std::vector<GameObject*> rendered_static_objects;
		ol_quadtree->CollectIntersect(rendered_static_objects, camera_frustum);

		for (const auto& object : rendered_static_objects)
		{
			ComponentMeshRenderer *object_mesh = static_cast<ComponentMeshRenderer*>(object->GetComponent(Component::ComponentType::MESH_RENDERER));
			culled_mesh_renderers.push_back(object_mesh);
		}

		// Then we add all dynamic objects culled using the aabbtree
		std::vector<GameObject*> rendered_dynamic_objects;
		ol_abbtree->GetIntersection(rendered_dynamic_objects, camera_frustum);

		for (const auto& object : rendered_dynamic_objects)
		{
			ComponentMeshRenderer *object_mesh = static_cast<ComponentMeshRenderer*>(object->GetComponent(Component::ComponentType::MESH_RENDERER));
			culled_mesh_renderers.push_back(object_mesh);
		}

		break;
	}

	default:
		break;
	}

	return culled_mesh_renderers;
}
