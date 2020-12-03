#include "OLOctTree.h"

#include "Main/GameObject.h"

#include <algorithm>

OLOctTree::~OLOctTree()
{
	Clear();
}

void OLOctTree::Create(AABB limits)
{
	Clear();
	root = new OLOctTreeNode(limits);
	flattened_tree.push_back(root);
}

void OLOctTree::Clear()
{
	for (const auto& node : flattened_tree)
	{
		delete node;
	}
	flattened_tree.clear();
	root = nullptr;
}

void OLOctTree::Insert(GameObject &game_object)
{
	if (!game_object.IsStatic())
	{
		return;
	}

	assert(root->box.Intersects(game_object.aabb.bounding_box));

	std::vector<OLOctTreeNode*> intersecting_leaves;
	FindLeaves(game_object.aabb.bounding_box, intersecting_leaves);
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
				std::vector<OLOctTreeNode*> generated_nodes;
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

void OLOctTree::CollectIntersect(std::vector<GameObject*> &game_objects, const Frustum& camera_frustum)
{
	root->CollectIntersect(game_objects, camera_frustum);
	const auto it = std::unique(game_objects.begin(), game_objects.end());
	game_objects.erase(it, game_objects.end());
}

void OLOctTree::FindLeaves(const AABB &game_object_aabb, std::vector<OLOctTreeNode*> &leaves) const
{
	for (const auto& node : flattened_tree)
	{
		if (node->IsLeaf() && node->box.Intersects(game_object_aabb))
		{
			leaves.push_back(node);
		}
	}
}

