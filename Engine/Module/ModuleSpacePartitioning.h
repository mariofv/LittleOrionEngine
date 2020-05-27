#ifndef _MODULESPACEPARTITIONING_H_
#define _MODULESPACEPARTITIONING_H_

#define ENGINE_EXPORTS

#include "Module.h"

#include "SpacePartition/OLQuadTree.h"
#include "SpacePartition/OLOctTree.h"
#include "SpacePartition/OLAABBTree.h"
#include <memory>

class ComponentCamera;
class GameObject;

const unsigned INITIAL_SIZE_AABBTREE = 10;

class ModuleSpacePartitioning : public Module
{
public:
	ModuleSpacePartitioning() = default;
	~ModuleSpacePartitioning() = default;

	bool Init();
	bool CleanUp();

	void GenerateQuadTree();
	void GenerateOctTree();
	void InsertAABBTree(GameObject* game_object);
	void RemoveAABBTree(GameObject* game_object);
	void UpdateAABBTree(GameObject* game_object);
	void ResetAABBTree();
	ENGINE_API void DrawAABBTree() const;
	void GetCullingMeshes(const ComponentCamera* camera) const;

private:
	std::unique_ptr<OLQuadTree> ol_quadtree = nullptr;
	std::unique_ptr<OLOctTree> ol_octtree = nullptr;
	std::unique_ptr<OLAABBTree> ol_abbtree = nullptr;

	friend class ModuleDebugDraw;
	friend class PanelConfiguration;
	friend class PanelScene;
};

#endif //_MODULESPACEPARTITIONING_H_