#ifndef _OLOCTTREE_H_
#define _OLOCTTREE_H_

#include "GameObject.h"
#include "OLOctTreeNode.h"

#include <MathGeoLib.h>

class OLOctTree
{
public:
	OLOctTree();
	~OLOctTree();

	void Create(AABB limits);
	void Clear();
	void Insert(GameObject &game_object);
	void CollectIntersect(std::vector<GameObject*> &game_objects, const ComponentCamera &camera);

private:
	OLOctTreeNode* FindLeaf(const AABB &game_object) const;

private:
	OLOctTreeNode *root = nullptr;
	std::vector<OLOctTreeNode*> flattened_tree;
	int bucket_size = 1;

};

#endif _OLOCTTREE_H_
