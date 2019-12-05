#include "OLQuadTree.h"
#include <queue>

OLQuadTree::OLQuadTree()
{
}

OLQuadTree::~OLQuadTree()
{
}

void OLQuadTree::Create(AABB limits)
{
	root = new OLQuadTreeNode(limits);
}

void OLQuadTree::Clear()
{

}

void OLQuadTree::Insert(const GameObject &game_object)
{
	if (root->box.Intersects(game_object.aabb.bounding_box))
	{
		OLQuadTreeNode *object_leaf = FindLeaf(game_object.aabb.bounding_box);
		if (object_leaf->objects.size() == bucket_size)
		{
			//object_leaf->Split();
		}
	}
}

void OLQuadTree::CollectIntersect(std::vector<GameObject*> &game_objects, const ComponentCamera &camera)
{

}

OLQuadTreeNode* OLQuadTree::FindLeaf(const AABB &game_object) const
{

	return root;
}