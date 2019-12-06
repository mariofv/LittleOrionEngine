#ifndef _OLQUADTREE_H_
#define _OLQUADTREE_H_

#include "GameObject.h"
#include "OLQuadTreeNode.h"

#include <MathGeoLib.h>

class OLQuadTree
{
public:
	OLQuadTree();
	~OLQuadTree();

	void Create(AABB limits);
	void Clear();
	void Insert(GameObject &game_object);
	void CollectIntersect(std::vector<GameObject*> &game_objects, const ComponentCamera &camera);

private:
	OLQuadTreeNode* FindLeaf(const AABB &game_object) const;

private:
	OLQuadTreeNode *root = nullptr;
	std::vector<OLQuadTreeNode*> flattened_tree;
	int bucket_size = 1;

};

#endif _OLQUADTREE_H_
