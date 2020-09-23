#ifndef _MODULESPACEPARTITIONING_H_
#define _MODULESPACEPARTITIONING_H_

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif


#include "Module.h"

#include "SpacePartition/OLQuadTree.h"
#include "SpacePartition/OLOctTree.h"
#include "SpacePartition/OLAABBTree.h"
#include <memory>

class ComponentCamera;
class ComponentMeshRenderer;
class GameObject;

const unsigned INITIAL_SIZE_AABBTREE = 10;

class ModuleSpacePartitioning : public Module
{
public:
	enum class CullingMode
	{
		NONE,
		FRUSTUM_CULLING,
		QUADTREE_CULLING,
		OCTTREE_CULLING,
		AABBTREE_CULLING,
		COMBINED_CULLING
	};

	ModuleSpacePartitioning() = default;
	~ModuleSpacePartitioning() = default;

	bool Init() override;

	void GenerateQuadTree();
	void GenerateOctTree();
	void InsertAABBTree(GameObject* game_object);
	void RemoveAABBTree(GameObject* game_object);
	void UpdateAABBTree(GameObject* game_object);

	void ResetAABBTree();
	ENGINE_API void DrawAABBTree() const;

	std::vector<ComponentMeshRenderer*> GetCullingMeshes(const ComponentCamera* camera, const std::vector<ComponentMeshRenderer*>& mesh_renderers, int culling_filters = 0) const;
	std::vector<ComponentMeshRenderer*> GetCullingMeshes(const Frustum& camera_frustum, const std::vector<ComponentMeshRenderer*>& mesh_renderers, int culling_filters = 0) const;

public:
	CullingMode culling_mode = CullingMode::FRUSTUM_CULLING;

private:
	std::unique_ptr<OLQuadTree> ol_quadtree = nullptr;
	std::unique_ptr<OLOctTree> ol_octtree = nullptr;
	std::unique_ptr<OLAABBTree> ol_abbtree = nullptr;

	friend class ModuleDebugDraw;
	friend class PanelConfiguration;
	friend class PanelScene;
};

#endif //_MODULESPACEPARTITIONING_H_