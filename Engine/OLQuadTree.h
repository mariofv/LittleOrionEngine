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

	void Create(AABB2D limits);
	void Clear();
	void Insert(GameObject &game_object);
	void CollectIntersect(std::vector<GameObject*> &game_objects, const ComponentCamera &camera);

public:
	std::vector<OLQuadTreeNode*> flattened_tree;
private:
	OLQuadTreeNode* FindLeaf(const AABB2D &game_object) const;

private:
	OLQuadTreeNode *root = nullptr;
	int bucket_size = 1;

};

#endif _OLQUADTREE_H_
