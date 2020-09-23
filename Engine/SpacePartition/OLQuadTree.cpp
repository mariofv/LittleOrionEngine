#include "OLQuadTree.h"

#include "Main/GameObject.h"

#include <algorithm>
#include <Brofiler/Brofiler.h>

OLQuadTree::~OLQuadTree()
{
	Clear();
}

void OLQuadTree::Create(AABB2D limits)
{
	Clear();
	root = new OLQuadTreeNode(limits);
	flattened_tree.push_back(root);
}

void OLQuadTree::Clear()
{
	for (const auto& node : flattened_tree)
	{
		delete node;
	}
	flattened_tree.clear();
	root = nullptr;
}

void OLQuadTree::Insert(GameObject &game_object)
{
	if (!game_object.IsStatic())
	{
		return;
	}
	assert(root->box.Intersects(game_object.aabb.bounding_box2D));

	std::vector<OLQuadTreeNode*> intersecting_leaves;
	FindLeaves(game_object.aabb.bounding_box2D, intersecting_leaves);
	assert(intersecting_leaves.size() > 0);

	bool reinsert = false;
	for (const auto& leaf : intersecting_leaves)
	{
		if (leaf->depth == max_depth)
		{
			leaf->InsertGameObject(&game_object);
		}
		else 
		{
			if (leaf->objects.size() == bucket_size)
			{
				std::vector<OLQuadTreeNode*> generated_nodes;
				leaf->Split(generated_nodes);
				flattened_tree.insert(flattened_tree.end(), generated_nodes.begin(), generated_nodes.end());
				reinsert = true;

			}
			else
			{
				leaf->InsertGameObject(&game_object);
			}
		}
	}

	if (reinsert)
	{
		Insert(game_object);
	}
}

void OLQuadTree::CollectIntersect(std::vector<GameObject*>& game_objects, const Frustum& camera_frustum)
{
	BROFILER_CATEGORY("OLQuadTree collect intersect", Profiler::Color::Yellow);
	root->CollectIntersect(game_objects, camera_frustum);
	const auto it = std::unique(game_objects.begin(), game_objects.end());
	game_objects.erase(it, game_objects.end());
}

void OLQuadTree::FindLeaves(const AABB2D &game_object_aabb, std::vector<OLQuadTreeNode*> &leaves) const
{
	for (const auto& node : flattened_tree)
	{
		if (node->IsLeaf() && node->box.Intersects(game_object_aabb))
		{
			leaves.push_back(node);
		}
	}
}


