#include "OLOctTree.h"
#include <queue>

OLOctTree::OLOctTree()
{
}

OLOctTree::~OLOctTree()
{
}

void OLOctTree::Create(AABB limits)
{
	root = new OLOctTreeNode(limits);
	flattened_tree.push_back(root);
}

void OLOctTree::Clear()
{

}

void OLOctTree::Insert(GameObject &game_object)
{
	if (root->box.Intersects(game_object.aabb.bounding_box))
	{
		OLOctTreeNode *object_leaf = FindLeaf(game_object.aabb.bounding_box); //TODO: What happens if the object is inside more one node?
		assert(object_leaf != nullptr);
		assert(object_leaf->objects.size() <= bucket_size);

		if (object_leaf->objects.size() == bucket_size)
		{
			std::vector<OLOctTreeNode*> generated_nodes;
			object_leaf->Split(generated_nodes);
			flattened_tree.insert(flattened_tree.end(), generated_nodes.begin(), generated_nodes.end());
			Insert(game_object);
		}
		else 
		{
			object_leaf->InsertGameObject(&game_object);
		}
	}
	//TODO: What happens if object is outside the tree?

}

void OLOctTree::CollectIntersect(std::vector<GameObject*> &game_objects, const ComponentCamera &camera)
{

}

OLOctTreeNode* OLOctTree::FindLeaf(const AABB &game_object_aabb) const
{
	for (auto &node : flattened_tree)
	{
		if (node->IsLeaf() && node->box.Intersects(game_object_aabb))
		{
			return root;
		}
	}

	return nullptr;
}